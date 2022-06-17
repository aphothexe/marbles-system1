#include <stdio.h>
#include <math.h>
#include <cstdint>

#include "pico/stdlib.h"

#include "plasma2040.hpp"

#include "common/pimoroni_common.hpp"
#include "rgbled.hpp"
#include "button.hpp"

/*
****LED STRIP STACKER GAME****
Button a Begins game

push button when the red and yellow leds cross each other.
see how far you get...
by Gee 'Rabid Inventor' Bartlett

*/

using namespace pimoroni;
using namespace plasma;

// Set how many LEDs you have
const uint N_LEDS = 40;
const uint REFRESH_DELAY = 100;

// Pick *one* LED type by uncommenting the relevant line below:

// APA102-style LEDs with Data/Clock lines. AKA DotStar
//APA102 led_strip(N_LEDS, pio0, 0, plasma2040::DAT, plasma2040::CLK);

// WS28X-style LEDs with a single signal line. AKA NeoPixel
// by default the WS2812 LED strip will be 400KHz, RGB with no white element
//WS2812 led_strip(N_LEDS, pio0, 0, plasma2040::DAT);

//Uncomment fo