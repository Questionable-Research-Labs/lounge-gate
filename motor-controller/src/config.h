#pragma once

typedef struct controller_config_s {
    float controller_stationary;
    float controller_deadzone;
    float controller_min;
    float controller_max;
    float motor_min;
    float motor_max;
    int animation_direction;
} controller_config_s;

#if defined(LEFT)
constexpr controller_config_s controller_config = {
    .controller_stationary = 0.43,
    .controller_deadzone = 0.04,
    .controller_min = 0.19,
    .controller_max = 1,
    .motor_min = 130,
    .motor_max = 200,
    .animation_direction = -1,
};
#else
// RIGHT ARDUINO UNO
constexpr controller_config_s controller_config = {
    .controller_stationary = 0.49,
    .controller_deadzone = 0.05,
    .controller_min = 0.21,
    .controller_max = 1,
    .motor_min = 130,
    .motor_max = 200,
    .animation_direction = -1,
};
#endif