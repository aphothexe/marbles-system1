#pragma once

#include "pico/stdlib.h"
#include "common/pimoroni_common.hpp"

using namespace pimoroni;

namespace motor {

  enum DecayMode {
    FAST_DECAY  = 0, //aka 'Coasting'
    SLOW_DECAY  = 1, //aka 'Braking'
  };

  class MotorState {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_SPEED_SCALE = 1.0f;        // The standard motor speed scale
    static constexpr float DEFAULT_ZEROPOINT = 0.0f;          // The standard motor zeropoint
    static constexpr float DEFAULT_DEADZONE = 0.05f;          // The standard motor deadzone

    static const DecayMode DEFAULT_DECAY_MODE = SLOW_DECAY;   // The standard motor decay behaviour
    static constexpr float DEFAULT_FREQUENCY = 25000.0f;      // The standard motor update rate
    static constexpr float MIN_FREQUENCY = 10.