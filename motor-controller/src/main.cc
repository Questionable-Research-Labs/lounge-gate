#include <Arduino.h>

constexpr int IN_PIN = A0;
constexpr int OUT_PIN = 3;
constexpr int RELAY_PIN = 13;

/** The midpoint of the input ADC, representing the stationary
 * position of the controller */
constexpr int FULL_ADC = 0b1111111111;
constexpr int HALF_ADC = FULL_ADC * 0.44;

constexpr int LOW_ADC = FULL_ADC * 0.19;
constexpr int HIGH_ADC = FULL_ADC;

/** The threshold either side of input ADC to be used as deadzone */
constexpr int ADC_DEADZONE = FULL_ADC * 0.05;

/** The minimum output of the PWM motor */
constexpr int MOTOR_OUT_MIN = 50;
/** The maximin output of the motor */
constexpr int MOTOR_OUT_MAX = 160;

/** Mode for relay to drive forwards */
constexpr int FORWARDS_STATE = LOW;
/** Mode for relay to drive forwards */
constexpr int BACKWARDS_STATE = HIGH;

enum MotorState { STATIONARY, FORWARDS, BACKWARDS };

MotorState previous_direction = STATIONARY;
unsigned long last_time;
MotorState last_change_direction = STATIONARY;
bool is_waiting = false;

/**
 * Calucalte the direction and PWM value from ADC reading
 * @param value The ADC reading, and where the PWM value will stored
 */
inline MotorState direction_from_value(int &value) {
    constexpr int back_det = HALF_ADC - ADC_DEADZONE;
    constexpr int forward_det = HALF_ADC + ADC_DEADZONE;
    constexpr int multiplier = MOTOR_OUT_MAX - MOTOR_OUT_MIN;

    if (value < back_det) {
        int range = back_det - LOW_ADC;
        value =
            ((float)(back_det - value) / range) * multiplier + MOTOR_OUT_MIN;
        return BACKWARDS;
    } else if (forward_det < value) {
        int range = HIGH_ADC - forward_det;
        value =
            ((float)(value - forward_det) / range) * multiplier + MOTOR_OUT_MIN;
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

    if (last_time - millis() > 1000) {
        is_waiting = false;
    }
}

void setup() {
    pinMode(IN_PIN, INPUT);
    pinMode(OUT_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    set_direction(FORWARDS);

    Serial.begin(9600);

    last_time = millis();
}

void loop() {
    delay(250);

    int value = analogRead(IN_PIN);
    Serial.print("Raw Value: ");
    Serial.print((float)value / (FULL_ADC));
    MotorState current_direction = direction_from_value(value);
    Serial.print(", Current Direction: ");
    Serial.print(current_direction);
    Serial.print(", Value: ");
    Serial.println(value);

    // if (current_direction == STATIONARY) {
    //     previous_direction = STATIONARY;
    //     setup_wait();
    //     analogWrite(OUT_PIN, 0);
    //     Serial.println("Stationary");
    // }

    // if (current_direction != previous_direction) {
    //     setup_wait();
    //     if (is_waiting) {
    //         Serial.println("Waiting");
    //         analogWrite(OUT_PIN, 0);
    //         return;
    //     }
    //     Serial.println("Wait finished");
    //     set_direction(current_direction);
    // }

    // if (current_direction != previous_direction) {
    //     Serial.println("Direction changed!");
    //     set_direction(current_direction);

    //     if (last_change_direction == current_direction) {
    //         // We were still waiting to change, so we can skip
    //         set_direction(current_direction);
    //         last_time = millis();
    //     } else {
    //         // Else we need to wait for the motor to stop
    //     }

    //     if (current_direction != STATIONARY) {
    //         last_time = millis();
    //         last_change_direction = previous_direction;
    //     }
    // }

    if (current_direction != previous_direction) {
        unsigned long time_diff = millis() - last_time;
        if (time_diff < 500) {
            Serial.print("POWER) Waiting for another ");
            Serial.print(time_diff);
            Serial.println("ms");

            // Cut power, and and go new direction
            value = 0;

        } else if (time_diff < 1000) {
            Serial.print("(ALL) Waiting for another ");
            Serial.print(time_diff);
            Serial.println("ms");

            // Cut power, and stick with previous direction
            value = 0;
            current_direction = previous_direction;

        } else {
            // Over a second
            last_time = millis();
        }
    } else {
        Serial.println("Same direction");
        last_time = millis();
    }

    previous_direction = current_direction;
    set_direction(current_direction);
    analogWrite(OUT_PIN, value);
    Serial.print("Final decision: ");
    Serial.print(current_direction);
    Serial.print(", Value: ");
    Serial.println(value);
}