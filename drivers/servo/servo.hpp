#pragma once

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo_state.hpp"

namespace servo {

  class Servo {
    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    uint servo_pin;
    ServoState state;
    pwm_config pwm_cfg;
    uint16_t pwm_period;
    float pwm_frequency;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    Servo(uint pin, CalibrationType default_type = ANGULAR, float freq = ServoState::DEFAULT_FREQUENCY);
    Servo(uint pin, const Calibration& calibration, float freq = ServoState::DEFAULT_FREQUENCY);
    ~Servo();

    //--------------------------------------------------
    // Methods
    //--------------------