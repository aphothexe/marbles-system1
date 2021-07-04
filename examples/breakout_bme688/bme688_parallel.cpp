#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "bme68x.hpp"
#include "common/pimoroni_i2c.hpp"

/*
Read a sequence of readings from the BME688 with given heat/duration profiles
Reading the full batch of readings will take some time. This seems to take ~10sec.
*/

using namespace pimoroni;

I2C i2c(BOARD::BREAKOUT_GARDEN);
BME68X bme68x(&i2c);

constexpr uint16_t profile_length = 10;

// Space for <profile_length> results
bme68x_data data[profile_length];

/* Heater temperature in degree Celsius */
uint16_t temps[profile_length] = { 320, 100, 100, 100, 200, 200, 200, 320, 320, 320 };

/* Multiplier to the shared heater duration */
uint16_t durations[profile_length] = { 5, 2, 10, 30, 5, 5, 5, 5, 5, 5 };


int main() {
#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  stdio_init_all();

  bme68x.init();

  while (1) {
    sleep_ms(1000);
    au