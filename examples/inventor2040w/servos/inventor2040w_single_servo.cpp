#include "pico/stdlib.h"

#include "inventor.hpp"

/*
Demonstrates how to control a single servo on Inventor 2040 W.
*/

using namespace inventor;

// How many sweeps of the servo to perform
const uint SWEEPS = 3;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between