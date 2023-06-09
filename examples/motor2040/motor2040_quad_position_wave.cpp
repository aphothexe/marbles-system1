
#include <cstdio>
#include "pico/stdlib.h"

#include "motor2040.hpp"
#include "button.hpp"
#include "pid.hpp"

/*
A demonstration of driving all four of Motor 2040's motor outputs between
positions, with the help of their attached encoders and PID control.

Press "Boot" to exit the program.
*/

using namespace plasma;
using namespace motor;
using namespace encoder;

// The gear ratio of the motor
constexpr float GEAR_RATIO = 50.0f;

// The counts per revolution of the motor's output shaft
constexpr float COUNTS_PER_REV = MMME_CPR * GEAR_RATIO;

// The scaling to apply to the motor's speed to match its real-world speed
constexpr float SPEED_SCALE = 5.4f;

// How many times to update the motor per second
const uint UPDATES = 100;
constexpr float UPDATE_RATE = 1.0f / (float)UPDATES;

// The time to travel between each random value
constexpr float TIME_FOR_EACH_MOVE = 2.0f;
const uint UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES;

// How many of the updates should be printed (i.e. 2 would be every other update)
const uint PRINT_DIVIDER = 4;

// The brightness of the RGB LED
constexpr float BRIGHTNESS = 0.4f;

// PID values
constexpr float POS_KP = 0.14f;   // Position proportional (P) gain
constexpr float POS_KI = 0.0f;    // Position integral (I) gain
constexpr float POS_KD = 0.002f;  // Position derivative (D) gain


// Create an array of motor pointers
const pin_pair motor_pins[] = {motor2040::MOTOR_A, motor2040::MOTOR_B,
                               motor2040::MOTOR_C, motor2040::MOTOR_D};
const uint NUM_MOTORS = count_of(motor_pins);
Motor *motors[NUM_MOTORS];

// Create an array of encoder pointers
const pin_pair encoder_pins[] = {motor2040::ENCODER_A, motor2040::ENCODER_B,
                                 motor2040::ENCODER_C, motor2040::ENCODER_D};
const char* ENCODER_NAMES[] = {"A", "B", "C", "D"};
const uint NUM_ENCODERS = count_of(encoder_pins);
Encoder *encoders[NUM_ENCODERS];

// Create the LED, using PIO 1 and State Machine 0
WS2812 led(motor2040::NUM_LEDS, pio1, 0, motor2040::LED_DATA);

// Create the user button
Button user_sw(motor2040::USER_SW);

// Create an array of PID pointers
PID pos_pids[NUM_MOTORS];


int main() {
  stdio_init_all();

  // Fill the arrays of motors, encoders, and pids, and initialise them
  for(auto i = 0u; i < NUM_MOTORS; i++) {
    motors[i] = new Motor(motor_pins[i], NORMAL_DIR, SPEED_SCALE);
    motors[i]->init();

    encoders[i] = new Encoder(pio0, i, encoder_pins[i], PIN_UNUSED, NORMAL_DIR, COUNTS_PER_REV, true);
    encoders[i]->init();

    pos_pids[i] = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE);
  }

  // Reverse the direction of the B and D motors and encoders
  motors[1]->direction(REVERSED_DIR);
  motors[3]->direction(REVERSED_DIR);
  encoders[1]->direction(REVERSED_DIR);
  encoders[3]->direction(REVERSED_DIR);

  // Start updating the LED
  led.start();

  // Enable all motors
  for(auto i = 0u; i < NUM_MOTORS; i++) {
    motors[i]->enable();
  }

  uint update = 0;
  uint print_count = 0;

  // Set the initial and end values
  float start_value = 0.0f;
  float end_value = 270.0f;

  Encoder::Capture captures[NUM_MOTORS];

  // Continually move the motor until the user button is pressed
  while(!user_sw.raw()) {

    // Capture the state of all the encoders
    for(auto i = 0u; i < NUM_MOTORS; i++) {
      captures[i] = encoders[i]->capture();
    }

    // Calculate how far along this movement to be
    float percent_along = (float)update / (float)UPDATES_PER_MOVE;

    for(auto i = 0u; i < NUM_MOTORS; i++) {
      // Move the motor between values using cosine
      pos_pids[i].setpoint = (((-cosf(percent_along * (float)M_PI) + 1.0) / 2.0) * (end_value - start_value)) + start_value;

      // Calculate the velocity to move the motor closer to the position setpoint
      float vel = pos_pids[i].calculate(captures[i].degrees(), captures[i].degrees_per_second());

      // Set the new motor driving speed
      motors[i]->speed(vel);
    }

    // Update the LED
    led.set_hsv(0, percent_along, 1.0f, BRIGHTNESS);

    // Print out the current motor values and their setpoints, but only on every multiple
    if(print_count == 0) {
      for(auto i = 0u; i < NUM_ENCODERS; i++) {
        printf("%s = %f, ", ENCODER_NAMES[i], captures[i].degrees());
      }
      printf("\n");
    }

    // Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER;

    update++;   // Move along in time

    // Have we reached the end of this movement?
    if(update >= UPDATES_PER_MOVE) {
      update = 0;  // Reset the counter

      // Swap the start and end values
      float temp = start_value;
      start_value = end_value;
      end_value = temp;
    }

    sleep_ms(UPDATE_RATE * 1000.0f);
  }

  // Stop all the motors
  for(auto m = 0u; m < NUM_MOTORS; m++) {
    motors[m]->disable();
  }

  // Turn off the LED
  led.clear();

  // Sleep a short time so the clear takes effect
  sleep_ms(100);
}