#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
An example of how to move a motor smoothly between random positions,
with the help of it's attached encoder and PID control.

Press "User" to exit the program.
*/

using namespace inventor;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
const Direction DIRECTION = NORMAL_DIR;

// The scaling to apply to the motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// How many 