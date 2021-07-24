#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"
#include "okcolor.hpp"

using namespace pimoroni;

PicoGraphics_PenRGB888 graphics(53, 11, nullptr);
GalacticUnicorn galactic_unicorn;

// left_channel_bin and right_channel_bin are in audio_samples.cpp
extern uint8_t left_channel_bin[];
extern uint32_t left_channel_bin_len;
extern uint8_t right_channel_bin[];
extern uint32_t right_channel_bin_len;

void gradient(uint8_t r, uint8_t g, uint8_t b) {
  for(int y = 0; y < 12; y++) {
    for(int x = 0; x < 53; x++) {
//      graphics.set_pen((r * x) / 52, (g * x) / 52, (b * x) / 52);
      graphics.set_pen(x, x, x);
      graphics.pixel(Point(x, y));
    }
  }
}

typedef void (*shader_func_t)(int ms, int x, int y);

void grid(int ms, int x, int y) {
  int v = (x + y + (ms / 1000)) % 2 == 0 ? 255 : 0;
  graphics.set_pen(v, v, v);
  graphics.pixel(Point(x, y));
}

void shader_fill(int ms, shader_func_t f) {
  for(int y = 0; y < 12; y++) {
    for(int x = 0; x < 53; x++) {
      f(ms, x, y);
    }
  } 
}

void outline_text(std::string text) {
  uint ms = to_ms_since_boot(get_absolute_time());

  graphics.set_font("bitmap8");
  uint8_t v = (sin(ms / 100.0f) + 1.0f) * 127.0f;
  uint w = graphics.measure_text(text, 1);

  int x = 53 / 2 - w / 2 + 1, y = 2;

  graphics.set_pen(0, 0, 0);
  graphics.text(text, Point(x - 1, y - 1), -1, 1);
  gra