#include <Arduino.h>
#include <animation.h>
#include <direction_controller.h>
#include <motor.h>
#include <config.h>

constexpr int IN_PIN = A0;
constexpr int OUT_PIN = 3;
constexpr int RELAY_PIN = 13;

/** The midpoint of the input ADC, repersenting the stationary
 * position of the controller */
constexpr int FULL_ADC = 0b1111111111;
constexpr int ADC_MIDPOINT = FULL_ADC * controller_config.controller_stationary;
/** The threshold either side of input ADC to be used as deadzone */
constexpr int ADC_DEADZONE = FULL_ADC * controller_config.controller_deadzone;

constexpr int ADC_MIN = FULL_ADC * controller_config.controller_min;
constexpr int ADC_MAX = FULL_ADC * controller_config.controller_max;

/** The minimum output of the PWM motor */
constexpr int MOTOR_OUT_MIN = controller_config.motor_min;

/** The maximim output of the motor */
// constexpr int MOTOR_OUT_MAX = 190;
constexpr int MOTOR_OUT_MAX = controller_config.motor_max;

/** Mode for relay to drive forwards */
constexpr int FORWARDS_STATE = HIGH;
/** Mode for relay to drive forwards */
constexpr int BACKWARDS_STATE = LOW;

MotorState previous_direction = STATIONARY;


/**
 * Calculate the direction and PWM value from ADC reading
 * @param value The ADC reading, and where the PWM value will stored
 */
tuned_value_result read_tuned_value(int value) {
    constexpr int back_det = ADC_MIDPOINT - ADC_DEADZONE;
    constexpr int forward_det = ADC_MIDPOINT + ADC_DEADZONE;
    constexpr int multiplier = MOTOR_OUT_MAX - MOTOR_OUT_MIN;

    MotorState current_direction = STATIONARY;
    float motorValue = 0;

    if (value < back_det) {
        const int range = back_det - ADC_MIN;
        motorValue = ((float)(back_det - value) / range);

        current_direction = BACKWARDS;
    } else if (forward_det < value) {
        const int range = ADC_MAX - forward_det;
        motorValue = ((float)(value - forward_det) / range);

        current_direction = FORWARDS;
    }


    int motorOutput = motorValue * multiplier + MOTOR_OUT_MIN;

    if (current_direction == STATIONARY) {
        motorOutput = 0;
    }

    Serial.println(motorOutput);


    tuned_value_result parsedResult = tuned_value_result { current_direction, motorOutput, motorValue };

    return direction_control(parsedResult);
}

/**
 * Change set the state of the relay
 */
inline void set_direction(MotorState direction) {
    switch (direction) {
    case STATIONARY:
        Serial.println("STATIONARY");
        return;
    case FORWARDS:
        digitalWrite(RELAY_PIN, FORWARDS_STATE);
        Serial.println("FORWARD");
        return;
    case BACKWARDS:
        digitalWrite(RELAY_PIN, BACKWARDS_STATE);
        Serial.println("BACKWARDS");
        return;
    }
}

void setup() {
    pinMode(IN_PIN, INPUT);
    pinMode(OUT_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    Serial.begin(9600);

    setup_neopixel();

    analogWrite(OUT_PIN, 0);
    set_direction(FORWARDS);

    run_startup_animation();
}

void loop() {
    int value = analogRead(IN_PIN);
    Serial.print("Raw Value: ");
    Serial.print((float)value / FULL_ADC);
    Serial.print(" | ");

    auto result = read_tuned_value(value);
	
	MotorState current_direction = result.direction;
	int motorOutput = result.motorOutput;

    Serial.print(motorOutput);
    Serial.print(" | ");
    Serial.println(current_direction);
    analogWrite(OUT_PIN, motorOutput);
    set_direction(result.direction);

    animation_loop(result.inputValue, current_direction);
}
