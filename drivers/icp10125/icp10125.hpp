#pragma once

#include <string>

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_common.hpp"
#include "common/pimoroni_i2c.hpp"

namespace pimoroni {

  class ICP10125 {
      public:
        enum meas_command {
            NORMAL = 0x6825,
            LOW_POWER = 0x609C,
            LOW_NOISE = 0x70DF,
            ULTRA_LOW_NOISE = 0x7866,
            /*NORMAL_T_FIRST = 0x6825,
            NORMAL_P_FIRST = 0x48A3,
            LOW_POWER_T_FIRST = 0x609C,
            LOW_POWER_P_FIRST = 0x401A,
            LOW_NOISE_T_FIRST = 0x70DF,
            LOW_NOISE_P_FIRST = 0x5059,
            ULN_T_FIRST = 0x7866,
            ULN_P_FIRST = 0x58E0*/
        };

        enum reading_status {
          OK = 0,
          CRC_FAIL = 1,
        };

        struct reading {
          float temperature;
          float pressure;
          reading_st