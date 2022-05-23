#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

#include "pico_unicorn.hpp"

using namespace pimoroni;

PicoUnicorn pico_unicorn;

int main() {

  pico_unicorn.init();

  bool a_pressed = false;
  bool b_pressed = false;
  bool x_pressed = false;
  bool y_pressed = false;

  uint32_t i = 0;
  while(true) {
    i = i + 1;

    if(pico_unicorn.is_pressed(pico_unicorn.A)) { a_pressed = true; }
    if(pico_unicorn.is_pressed(pico_unicorn.B)) { b_pressed = true; }
    if(pico_unicorn.is_pressed(pico_unicorn.X)) { x_pressed = true; }
    if(pico_unicorn.is_pressed(pico_unicorn.Y)) { y_pressed = true; }

    pico_unicorn.clear();

    if(a_pressed & b_pressed & x_pressed & y_pressed) {
      for(int y = 0; y < 7; y++) {
        for(int x = 0; x < 16; x++) {
          int v = (x + y + (i / 100)) % 2 == 0 ? 0 : 100;
          pico_unicorn.set_pixel(x, y, v);
        }
      }
    } else {
      // button test phase
      float pulse = fmod(float(i) / 20.0f, M_PI * 2.0f);
      int v = int((sin(pulse) * 50.0f) + 50.0f);
      if(a_pressed) {
