#include <direction_controller.h>

MotorState previous_confirmed_direction = STATIONARY;
bool is_waiting = false;
unsigned long last_direction_change_ms = millis();

unsigned long total_delay = 1000; 

// Going straight from forward to backward, or vice versa, is a bad idea
// as it can cause strong EMF which can damage the motor controller
// It could also quite effectively fling the passengers out of the couch
tuned_value_result direction_control(tuned_value_result input) {
    if (input.direction == STATIONARY) {
        return input;
    }

    if (input.direction != previous_confirmed_direction) {
        if (is_waiting) {
            if (millis() - last_direction_change_ms > total_delay) {
                is_waiting = false;
            }
        } else {
            is_waiting = true;
        }
    } else {
        is_waiting = false;
    }

    if (is_waiting) {
        input.direction = STATIONARY;
        input.motorOutput = 0;
    } else {
        previous_confirmed_direction = input.direction;
        last_direction_change_ms = millis();
    }
    
    

    return input;
}
