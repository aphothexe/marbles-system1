#pragma once

#include "hardware/pio.h"
#include "common/pimoroni_common.hpp"

using namespace pimoroni;

namespace encoder {
  static constexpr float MMME_CPR = 12.0f;
  static constexpr float ROTARY_CPR = 24.0f;

  class Encoder {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static constexpr float DEFAULT_COUNTS_PER_REV   = ROTARY_CPR;
    static const bool DEFAULT_COUNT_MICROSTEPS      = false;
    static const uint16_t DEFAULT_FREQ_DIVIDER      = 1;

  private:
    static const uint32_t STATE_A_MASK      = 0x80000000;
    static const uint32_t STATE_B_MASK      = 0x40000000;
    static const uint32_t STATE_A_LAST_MASK = 0x20000000;
    static const uint32_t STATE_B_LAST_MASK = 0x10000000;

    static const uint32_t STATES_MASK = STATE_A_MASK | STATE_B_MASK |
                                        STATE_A_LAST_MASK | STATE_B_LAST_MASK;

    static const uint32_t TIME_MASK   = 0x0fffffff;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  private:
    enum StepDir {
      NO_DIR    = 0,
      INCREASING = 1,
      DECREASING = -1,
    };

    enum MicroStep : uint8_t {
      MICROSTEP_0 = 0b00,
      MICROSTEP_1 = 0b10,
      MICROSTEP_2 = 0b11,
      MICROSTEP_3 = 0b01,
    };


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    class Capture {
        //--------------------------------------------------
        // Variables
        //--------------------------------------------------
      private:
        int32_t captured_count;
        int32_t captured_delta;
        float captured_frequency;
        float counts_per_rev;


        //--------------------------------------------------
        // Constructors
        //----------------------