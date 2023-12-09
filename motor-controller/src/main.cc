#include <Arduino.h>

constexpr int IN_PIN = A0;
constexpr int OUT_PIN = 3;
constexpr int PWM_THRESHOLD = 0b0111111111;

void setup() {
    pinMode(IN_PIN, INPUT);
    pinMode(OUT_PIN, OUTPUT);

    Serial.begin(9600);
}

void loop() {
    int value = analogRead(IN_PIN);
    
    if (value > PWM_THRESHOLD) {
        int pwm_value = (((float) value - (float) PWM_THRESHOLD) / PWM_THRESHOLD) * 160;
        Serial.print("Value is ");
        Serial.println(pwm_value);
        analogWrite(OUT_PIN, pwm_value);
    } else {
        analogWrite(OUT_PIN, 0);
    }
}
