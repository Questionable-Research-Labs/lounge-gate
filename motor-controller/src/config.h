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
    .controller_stationary = 0.50,
    .controller_deadzone = 0.02,
    .controller_min = 0.2,
    .controller_max = 1,
    .motor_min = 130,
    .motor_max = 180,
    .animation_direction = 1,
};
#else
// RIGHT ARDUINO UNO
constexpr controller_config_s controller_config = {
    .controller_stationary = 0.50,
    .controller_deadzone = 0.02,
    .controller_min = 0.2,
    .controller_max = 1,
    .motor_min = 130,
    .motor_max = 180,
    .animation_direction = -1,
};
#endif