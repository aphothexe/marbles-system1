#include "pico/stdlib.h"

#include "motor2040.hpp"

/*
Demonstrates how to create multiple Motor objects and control them together.
*/

using namespace motor;

// How many sweeps of the motors to perform
const uint SWEEPS = 2;

// The number of discrete sweep steps
const uint STEPS = 10;

// The time in milliseconds between each step of the sequence
const uint STEPS_INTERVAL_MS = 500;

// How far from zero to drive the motors when sweeping
constexpr float SPEED_EXTENT = 1.0f;

// Create a motor cluster, using PIO 0 and State Machine 0
const pin_pair motor_pins[] = {motor2040::MOTOR_A, motor2040::MOTOR_B,
                               motor2040::MOTOR_C, motor2040::MOTOR_D};
const uint NUM_MOTORS = count_of(motor_pins);
Moto