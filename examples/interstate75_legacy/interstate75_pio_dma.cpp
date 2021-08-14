#include <stdio.h>
#include <math.h>
#include <cstdint>
#include <cstring>

#include "pico/stdlib.h"

#include "common/pimoroni_common.hpp"

#include "hub75.hpp"

using namespace pimoroni;

// Display size in pixels
// Should be either 64x64 or 32x32 but perhaps 64x32 an other sizes will work.
// Note: this example uses only 5 address lines so it's limited to 32*2 pixels.
const uint8_t