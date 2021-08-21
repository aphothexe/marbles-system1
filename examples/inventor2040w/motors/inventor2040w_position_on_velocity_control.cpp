
#include <cstdio>
#include "pico/stdlib.h"

#include "inventor.hpp"
#include "pid.hpp"

/*
An example of how to move a motor smoothly between random positions,
with velocity limits, with the help of it's attached encoder and PID control.

Press "User" to exit the program.
*/

using namespace inventor;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
const Direction DIRECTION = NORMAL_DIR;

// The scaling to apply to the motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// How many times to update the motor per second
const uint UPDATES = 100;
constexpr float UPDATE_RATE = 1.0f / (float)UPDATES;

// The time to travel between each random value, in seconds
constexpr float TIME_FOR_EACH_MOVE = 1.0f;
const uint UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES;

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 4;

// Multipliers for the different printed values, so they appear nicely on the Thonny plotter
constexpr float ACC_PRINT_SCALE = 2.0f;     // Acceleration multiplier
constexpr float SPD_PRINT_SCALE = 40.0f;    // Driving Speed multipler

// How far from zero to move the motor, in degrees
constexpr float POSITION_EXTENT = 180.0f;

// The maximum speed to move the motor at, in revolutions per second
constexpr float MAX_SPEED = 1.0f;

// The interpolating mode between setpoints. STEP (0), LINEAR (1), COSINE (2)
const uint INTERP_MODE = 0;

// PID values
constexpr float POS_KP = 0.025f;  // Position proportional (P) gain
constexpr float POS_KI = 0.0f;    // Position integral (I) gain
constexpr float POS_KD = 0.0f;    // Position derivative (D) gain

constexpr float VEL_KP = 30.0f;   // Velocity proportional (P) gain
constexpr float VEL_KI = 0.0f;    // Velocity integral (I) gain
constexpr float VEL_KD = 0.4f;    // Velocity derivative (D) gain


// Create a new Inventor2040W
Inventor2040W board(GEAR_RATIO);

// Create PID object for both position and velocity control
PID pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE);
PID vel_pid = PID(VEL_KP, VEL_KI, VEL_KD, UPDATE_RATE);


int main() {
  stdio_init_all();

  // Attempt to initialise the board
  if(board.init()) {

    // Access the motor and encoder from Inventor
    Motor& m = board.motors[MOTOR_A];
    Encoder& enc = board.encoders[MOTOR_A];

    // Set the motor's speed scale
    m.speed_scale(SPEED_SCALE);

    // Set the motor and encoder's direction