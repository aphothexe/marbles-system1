#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "breakout_encoder.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
Press "B" to enable cycling.
Press "A" to change the encoder mode.
Press "Boot" to reset the effects back to default.
*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 30;

// The speed that the LEDs will start cycling at
const int16_t DEFAULT_SPEED = 20;

// The hue (in degrees) that the LEDs will start at
const int16_t DEFAULT_HUE = 0;

// The angle (in degrees) from the hue, that the LEDs will end at
const int16_t DEFAULT_ANGLE = 120;

// The brightness (between 0 and 31) to set the LEDs to
const int16_t DEFAULT_BRIGHTNESS = 16;

// How many times the LEDs will be updated per second
const uint UPDATES = 60;


// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);


Button user_sw(plasma2040::USER_SW, Polarity::ACTIVE_LOW, 0);
Button button_a(plasma2040::BUTTON_A, Polarity::ACTIVE_LOW, 0);
Button button_b(plasma2040::BUTTON_B, Polarity::ACTIVE_LOW, 0);

RGBLED led(plasma2040::LED_R, plasma2040::LED_G, plasma2040::LED_B);

I2C i2c(BOARD::PICO_EXPLORER);
BreakoutEncoder enc(&i2c);

enum ENCODER_MODE {
  COLOUR,
  ANGLE,
  BRIGHTNESS,
  SPEED
};

float wrap(float v, float min, float max) {
  if(v <= min)
    v += (max - min);

  if(v > max)
    v -= (max - min);

  return v;
}


void colour_cycle(float hue, float t, float angle) {
  t /= 200.0f;

  for(auto i = 0u; i < led_strip.num_leds; ++i) {
    float percent_along = (float)i / led_strip.num_leds;
    float offset = sinf((percent_along + 0.5f + t) * M_PI) * angle;
    float h = wrap((hue + offset) / 360.0f, 0.0f, 1.0f);
    led_strip.set_hsv(i, h, 1.0f, 1.0f);
  }
}

void speed_gauge(uint v, uint vmax = 100) {
  uint light_pixels = led_strip.num_leds * v / vmax;

  for(auto i = 0u; i < led_strip.num_leds; ++i) {
    if(i < light_pixels) {
      led_strip.set_rgb(i, 0, 255, 0);
    }
    else {
      led_strip.set_rgb(i, 255, 0, 0);
    }
  }
}

void brightness_gauge(uint v, uint vmax = 100) {
  uint light_pixels = led_strip.num_leds * v / vmax;

  for(auto i = 0u; i < led_strip.num_leds; ++i) {
    if(i < light_pixels) {
      led_strip.set_rgb(i, 64, 64, 64);
    }
    else {
      led_strip.set_rgb(i, 0, 0, 0);
    }
  }
}

int main() {
  st