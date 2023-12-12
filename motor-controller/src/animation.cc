#include <animation.h>

#define GREEN_COLOUR 19, 147, 78
#define CLEAR_COLOUR 0, 0, 0
#define WHITE_COLOUR 255, 255, 255

#define FORWARD_COLOUR 166, 0, 255
#define BACKWARD_COLOUR 255, 89, 0

constexpr int NEOPIXEL_PIN = 11;
constexpr int LED_COUNT = 24;

float headlight_value = 0;
float headlight_step = 0.1;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEOPIXEL_PIN, NEO_GBR + NEO_KHZ800);

void setup_neopixel() {
    strip.begin();
    strip.setBrightness(100);
    strip.clear();
    strip.show();
}

void run_startup_animation() {
    Serial.println("Running startup animation");
    color_wipe(strip.Color(GREEN_COLOUR), 50);
    color_wipe(strip.Color(CLEAR_COLOUR), 50);
    Serial.println("Finished startup animation");
}

void animation_loop(float value, MotorState current_direction) {
    // Operations are in ram, so it won't cause flickering if we later overwrite
    strip.clear();

    // Calculate headlight lighting
    float headlightChange = (current_direction == STATIONARY) ? headlight_step : -headlight_step;

    headlight_value += headlightChange;
    if (headlight_value < 0) {
        headlight_value = 0;
    } else if (headlight_value > 1) {
        headlight_value = 1;
    }

    if (headlight_value != 0) {
        int headlightCenter = (LED_COUNT) / 2;
        int headlightWidth = LED_COUNT / 2;
        // Grow the headlight from the center outwards
        int headlightStartIndex = headlightCenter - (headlightWidth * headlight_value / 2);
        int headlightEndIndex = headlightCenter + (headlightWidth * headlight_value / 2);
        strip.fill(strip.Color(WHITE_COLOUR), headlightStartIndex, headlightEndIndex - headlightStartIndex);


        Serial.print("Headlight value: ");
        Serial.print(headlight_value);
        Serial.print(" | ");
        Serial.print("Headlight start: ");
        Serial.print(headlightStartIndex);
        Serial.print(" | ");
        Serial.print("Headlight end: ");
        Serial.println(headlightEndIndex);

    }


    if (current_direction == STATIONARY) {
        strip.show();
        return;
    }

    // Value is between 0 and 1
    uint32_t colour;
    if (current_direction == FORWARDS) {
        colour = strip.Color(FORWARD_COLOUR);
    } else if (current_direction == BACKWARDS) {
        colour = strip.Color(BACKWARD_COLOUR);
    }


    int num_pixels = ceil(value * LED_COUNT);
    for (int i = 0; i < num_pixels; i++) {
        strip.setPixelColor(i, colour);
    }
    
    strip.show();
}

void color_wipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
