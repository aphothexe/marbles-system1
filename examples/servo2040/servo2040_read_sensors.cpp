#include <cstdio>
#include "pico/stdlib.h"

#include "servo2040.hpp"
#include "analogmux.hpp"
#include "analog.hpp"
#include "button.hpp"

/*
Shows how to initialise and read the 6 external
and 2 internal sensors of Servo 2040.

Press "Boot" to exit the program.
*/

using namespace servo;

// Set up the shared analog inputs
Analog sen_adc = Analog(servo2040::SHARED_ADC);
Analog vol_adc = Analog(servo2040::SHARED_ADC, servo2040::VOLTAGE_GAIN);
Analog cur_adc = Analog(servo2040::SHARED_ADC, servo2040::CURRENT_GAIN,
                        servo2040::SHUNT_RESISTOR, servo2040::CURRENT_OFFSET);

// Set up the analog multiplexer, including the pin for controlling pull-up/pull-down
AnalogMux mux = AnalogMux(servo2040::ADC_ADDR_0, servo2040::ADC_ADDR_1, servo2040::ADC_ADDR_2,
                          PIN_UNUSED, servo2040::SHARED_ADC)