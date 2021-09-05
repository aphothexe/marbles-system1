#include <cstdio>
#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "analogmux.hpp"
#include "analog.hpp"
#include "button.hpp"

/*
Shows how to initialise and read the 2 external
and 6 internal sensors of Motor 2040.

Press "Boot" to exit the program.
*/

using namespace mo