#include <Arduino.h>

constexpr int IN_PIN = A0;
constexpr int OUT_PIN = 3;
constexpr int RELAY_PIN = 13;

/** The midpoint of the input ADC, repersenting the stationary
 * position of the controller */
constexpr int HALF_ADC = 0b0111111111;
/** The threshold either side of input ADC to be used as deadzone */
constexpr int ADC_DEADZONE = 50;

/** The minimum output of the PWM motor */
constexpr int MOTOR_OUT_MIN = 50;
/** The maximim output of the motor */
constexpr int MOTOR_OUT_MAX = 160;

/** Mode for relay to drive forwards */
constexpr int FORWARDS_STATE = HIGH;
/** Mode for relay to drive forwards */
constexpr int BACKWARDS_STATE = LOW;

enum MotorState { STATIONARY, FORWARDS, BACKWARDS };

MotorState previous_direction = STATIONARY;
unsigned long last_time;
bool is_waiting = false;

/**
 * Calucalte the direction and PWM value from ADC reading
 * @param value The ADC reading, and where the PWM value will stored
 */
inline MotorState direction_from_value(int &value) {
  constexpr int back_det = HALF_ADC - ADC_DEADZONE;
  constexpr int forward_det = HALF_ADC + ADC_DEADZONE;
  constexpr float range = HALF_ADC - ADC_DEADZONE;
  constexpr int multiplier = MOTOR_OUT_MAX - MOTOR_OUT_MIN;

  if (value < back_det) {
    value = ((float)(back_det - value) / range) * multiplier + MOTOR_OUT_MIN;
    return BACKWARDS;
  } else if (forward_det < value) {
    value = ((float)(value - forward_det) / range) * multiplier + MOTOR_OUT_MIN;
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

void setup_wait() {
  if (!is_waiting) {
    is_waiting = true;
    last_time = millis();
    return;
  }

  if (last_time > 1000) {
    is_waiting = false;
  }
}

void setup() {
  pinMode(IN_PIN, INPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  Serial.begin(9600);

  last_time = millis();
}

void loop() {
  int value = analogRead(IN_PIN);
  MotorState current_direction = direction_from_value(value);

  if (current_direction == STATIONARY) {
    previous_direction = STATIONARY;
    setup_wait();
    analogWrite(OUT_PIN, 0);
  }

  if (current_direction != previous_direction) {
    setup_wait();
    if (is_waiting) {
      analogWrite(OUT_PIN, 0);
      return;
    }
    set_direction(current_direction);
  }

  analogWrite(OUT_PIN, value);
}
