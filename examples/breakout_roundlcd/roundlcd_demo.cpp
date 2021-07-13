#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "time.h"

// Place a 1.3 Round SPI LCD in the *front* slot of breakout garden.

using namespace pimoroni;


const int WIDTH = 240;
const int HEIGHT = 240;

ST7789 st7789(WIDTH, HEIGHT, ROTATE_0, true, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

constexpr float RADIUS = WIDTH / 2;

Pen from_hsv(float h, float s, float v) {
  uint8_t r = 0, g = 0, b = 0;

  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; b