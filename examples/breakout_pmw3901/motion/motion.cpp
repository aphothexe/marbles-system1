#include <stdio.h>
#include "pico/stdlib.h"

// Uncomment the below line to switch from the PMW3901 to the PAA5100
//#define USE_PAA5100

#ifndef USE_PAA5100
  #include "breakout_pmw3901.hpp"
#else
  #include "breakout_paa5100.hpp"
#endif

using names