// Reference page
// https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#managing-your-own-output

#include "FastLED.h"

#define NUM_LEDS 16

CRGB leds_ws2812[NUM_LEDS];
CRGB leds_sk6812[NUM_LEDS];

uint8_t scale_ws2812;
uint8_t scale_sk6812;

void setup() {
    FastLED.addLeds<WS2812, 32>(leds_ws2812, NUM_LEDS);
    FastLED.addLeds<SK6812, 33>(leds_sk6812, NUM_LEDS);
}

void loop() {
    float t = (float)millis() / 1000.f;
    scale_ws2812 = 255.f * sin(t);
    scale_sk6812 = 255.f * cos(t);

    fill_solid(leds_ws2812, NUM_LEDS, CRGB::White);
    FastLED[0].showLeds(scale_ws2812);

    fill_solid(leds_sk6812, NUM_LEDS, CRGB::White);
    FastLED[1].showLeds(scale_sk6812);
}
