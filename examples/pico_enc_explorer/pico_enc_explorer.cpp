#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <iomanip>

#include "pico_explorer.hpp"
#include "breakout_encoder.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

Pen BLACK = graphics.create_pen(0, 0, 0);
Pen RED = graphics.create_pen(255, 0, 0);
Pen GREEN = graphics.create_pen(0, 255, 0);
Pen BLUE = graphics.create_pen(0, 0, 255);

static const uint8_t STEPS_PER_REV = 24;

BreakoutEncoder enc;
bool toggle = false;

void count_changed(int16_t count) {
  printf("Count: %d\n", count);
  float h = (count % STEPS_PER_REV) / (float)STEPS_PER_REV;
  RGB p = RGB::from_hsv(h, 1.0f, 1.0f);
  enc.set_led(p.r, p.g, p.b);

  graphics.set_pen(BLACK);
  graphics.clear();

  {
    graphics.set_pen(RED);
    std::ostringstream ss;
    ss << "R = ";
    ss << (int)(p.r);
    std::string s(ss.str());
    graphics.text(s, Point(10, 10), 220, 6);
  }

  {
    graphics.set_pen(GREEN);
    std::ostringstream ss;
    ss << "G = ";
    ss << (int)(p.g);
    std::string s(ss.str());
    graphics.text(s, Point(10, 70), 220, 6);
  }

  {
    graphics.set_pen(BLUE);
    std::ostringstream ss;
    ss << "B = ";
    ss << (int)(p.b);
    std::string s(ss.str());
    graphics.text(s, Point(10, 130), 220, 6);
  }

  {
    // Shouldn't reall