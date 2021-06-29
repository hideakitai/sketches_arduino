#include "FastLED.h"

#define NUM_LEDS 16

CRGB leds[NUM_LEDS];

void setup() {
    FastLED.addLeds<WS2812, 3>(leds, NUM_LEDS);

    FastLED.setBrightness(255);
    FastLED.setDither(BINARY_DITHER);  // explicitly enable dithering

    // disable blocking in FastLED.show()
    // https://github.com/FastLED/FastLED/blob/master/FastLED.cpp#L47
    FastLED.setMaxRefreshRate(0);  // disable blocking in FastLED.show()

    // FastLED color correction & temperature settings
    // these settings are disabled unless you define
    // #define FASTLED_SEQUENCER_ENABLE_COLOR_CORRECTIONS
    // this skips correction procedure and makes a bit faster
    //
    // FastLED final color is calcurated as:
    // final color = raw color * correction * temperature * brightness
    // https://github.com/FastLED/FastLED/blob/master/controller.h#L150
    // you can disable these correction if you define
    // #define NO_CORRECTION 1 // disable FastLED setCorrection & setTemperature
    //
    // for color correction / temperature reference
    // https://github.com/FastLED/FastLED/blob/master/color.h
    // FastLED.setCorrection(TypicalLEDStrip); // 255, 176, 240
    // FastLED.setTemperature(DirectSunlight); // 255, 255, 255
    FastLED.setCorrection(CRGB(255, 255, 255));
    FastLED.setTemperature(CRGB(255, 255, 255));
}

void loop() {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
}
