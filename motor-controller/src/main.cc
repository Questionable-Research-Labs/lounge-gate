#include <Arduino.h>

constexpr int IN_PIN = A0;
constexpr int OUT_PIN = 3;
constexpr int RELAY_PIN = 13;

/** The midpoint of the input ADC, repersenting the stationary
 * position of the controller */
constexpr int FULL_ADC = 0b1111111111;
// constexpr int ADC_MIDPOINT = FULL_ADC * 0.44;
constexpr int ADC_MIDPOINT = FULL_ADC * 0.49;
/** The threshold either side of input ADC to be used as deadzone */
constexpr int ADC_DEADZONE = FULL_ADC * 0.1;

constexpr int ADC_MIN = FULL_ADC * 0.2;
constexpr int ADC_MAX = FULL_ADC * 1;

/** The minimum output of the PWM motor */
constexpr int MOTOR_OUT_MIN = 114;
// constexpr int MOTOR_OUT_MIN = 103;

/** The maximim output of the motor */
// constexpr int MOTOR_OUT_MAX = 140;
constexpr int MOTOR_OUT_MAX = 155;

/** Mode for relay to drive forwards */
constexpr int FORWARDS_STATE = HIGH;
/** Mode for relay to drive forwards */
constexpr int BACKWARDS_STATE = LOW;

enum MotorState { STATIONARY, FORWARDS, BACKWARDS };

MotorState previous_direction = STATIONARY;
bool is_waiting = false;

/**
 * Calucalte the direction and PWM value from ADC reading
 * @param value The ADC reading, and where the PWM value will stored
 */
inline MotorState direction_from_value(int &value) {
  constexpr int back_det = ADC_MIDPOINT - ADC_DEADZONE;
  constexpr int forward_det = ADC_MIDPOINT + ADC_DEADZONE;
  constexpr int multiplier = MOTOR_OUT_MAX - MOTOR_OUT_MIN;

  if (value < back_det) {
    const int range = back_det - ADC_MIN;
    value = ((float)(back_det - value) / range) * multiplier + MOTOR_OUT_MIN;
    Serial.println(value);
    return BACKWARDS;
  } else if (forward_det < value) {
    const int range = ADC_MAX - forward_det;
    value = ((float)(value - forward_det) / range) * multiplier + MOTOR_OUT_MIN;
    Serial.println(value);

    return FORWARDS;
  }

  value = 0;
  return STATIONARY;
}

/**
 * Change set the state of the relay
 */
inline void set_direction(MotorState direction) {
  switch (direction) {
  case STATIONARY:
    return;
  case FORWARDS:
    digitalWrite(RELAY_PIN, FORWARDS_STATE);
    return;
  case BACKWARDS:
    digitalWrite(RELAY_PIN, BACKWARDS_STATE);
    return;
  }
}


void setup() {
  pinMode(IN_PIN, INPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  Serial.begin(9600);

}

void loop() {
  int value = analogRead(IN_PIN);
  Serial.print("Raw Value: ");
  Serial.print((float)value / FULL_ADC);
  Serial.print(" | ");
  MotorState current_direction = direction_from_value(value);
  Serial.print(value);
  Serial.print(" | ");
  Serial.println(current_direction);
  analogWrite(OUT_PIN, value);
}
