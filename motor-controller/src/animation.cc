#include <animation.h>

#define GREEN_COLOUR 0, 191, 0
#define CLEAR_COLOUR 0, 0, 0
#define WHITE_COLOUR 0, 0, 255

#define BACKGROUND_COLOUR 20, 0, 10

#define FORWARD_COLOUR 166, 0, 255
#define BACKWARD_COLOUR 255, 89, 0

constexpr int NEOPIXEL_PIN = 11;
constexpr int LED_COUNT = 24;

int current_center = 0;
int direction = -1;

float headlight_value = 0;
float headlight_step = 0.1;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup_neopixel() {
    strip.begin();
    strip.setBrightness(100);
    strip.clear();
    strip.show();
}

void run_startup_animation() {
    Serial.println("Running startup animation");
    color_wipe(strip.Color(BACKGROUND_COLOUR), 50);
    Serial.println("Finished startup animation");
}

void animation_loop(float value, MotorState current_direction) {
    // Operations are in ram, so it won't cause flickering if we later overwrite
    strip.fill(strip.Color(BACKGROUND_COLOUR));

    current_center += direction;
    if (current_center >= LED_COUNT) {
        current_center = 0;
    } else if (current_center < 0) {
        current_center = LED_COUNT - 1;
    }

    // Calculate headlight lighting
    float headlightChange = (current_direction == STATIONARY) ? headlight_step : -headlight_step;

    headlight_value += headlightChange;
    if (headlight_value < 0) {
        headlight_value = 0;
    } else if (headlight_value > 1) {
        headlight_value = 1;
    }

    if (headlight_value != 0) {
        int headlightCenter = current_center;
        int headlightWidth = LED_COUNT / 4;
        // Grow the headlight from the center outwards
        int headlightStartIndex = headlightCenter - (headlightWidth * headlight_value / 2);
        int headlightEndIndex = headlightCenter + (headlightWidth * headlight_value / 2);
        circular_fill(strip.Color(WHITE_COLOUR), headlightStartIndex, headlightEndIndex);


        // Serial.print("Headlight value: ");
        // Serial.print(headlight_value);
        // Serial.print(" | ");
        // Serial.print("Headlight start: ");
        // Serial.print(headlightStartIndex);
        // Serial.print(" | ");
        // Serial.print("Headlight end: ");
        // Serial.println(headlightEndIndex);

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
    circular_fill(colour, current_center - num_pixels / 2, current_center + num_pixels / 2);
    
    strip.show();
}

void color_wipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void circular_fill(uint32_t c, int start, int end) {
    int edge_range = (end - start) / 2;

    int colourRed = c >> 16;    
    int colourGreen = (c >> 8) & 0xFF;
    int colourBlue = c & 0xFF;

    int background[] = { BACKGROUND_COLOUR };
    auto [backgroundRed, backgroundGreen, backgroundBlue] = background;

    for (int i = start; i < end; i++) {
        // Serial.print("Index: ");
        // Serial.print(i);
        float edge_distance = ((float)min(abs(i - (start - 1)), abs(i - (end))) ) / edge_range;

        edge_distance = min(edge_distance, 1.0);
        edge_distance /= 0.1;

        // Edge distance is the mix factor between the colour and the background colour
        uint32_t newColour = strip.Color(
            (int)(colourRed * edge_distance + backgroundRed * (1 - edge_distance)),
            (int)(colourGreen * edge_distance + backgroundGreen * (1 - edge_distance)),
            (int)(colourBlue * edge_distance + backgroundBlue * (1 - edge_distance))
        );

        if (i < 0) {
            strip.setPixelColor(LED_COUNT + i, newColour);
        } else if (i >= LED_COUNT) {
            strip.setPixelColor(i - LED_COUNT, newColour);
        } else {
            strip.setPixelColor(i, newColour);
        }
    }
}