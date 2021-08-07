#include <math.h>
#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/hub75/hub75.hpp"

using namespace pimoroni;


// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.
//If the display looks streaky or corrupted then uncomment one of the other initalisers

//Works with our 32x32 panels https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=35962488650 https://shop.pimoroni.com/products/rgb-led-matrix-panel?variant=19321740999
Hub75 hub75(32, 32, nullptr, PANEL_GENERIC, false);
//or using 2 of these panels
//Hub75 hub75(64, 32, nullptr, PANEL_GENERIC, false);

//and 64x32 panel https://shop.pimoroni.com/products/rgb-led-matrix-