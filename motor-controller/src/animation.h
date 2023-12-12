#include "motor.h"
#include <Adafruit_NeoPixel.h>

void setup_neopixel();

void run_startup_animation();

void animation_loop(float value, MotorState current_direction);

void color_wipe(uint32_t c, uint8_t wait);