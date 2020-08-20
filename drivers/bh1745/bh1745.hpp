#pragma once

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "common/pimoroni_i2c.hpp"

#define BH1745_REG_SYSTEM_CONTROL 0x40
#define BH1745_REG_MODE_CONTROL1 0x41
#define BH1745_REG_MODE_CONTROL2 0x42
#define BH1745_REG_MODE_CONTROL3 0x44
#define BH1745_REG_COLOUR_DATA 0x50
#define BH1745_REG_DINT_DATA 0x58
#define BH1745_REG_INTERRUPT 0x60
#define BH1745_REG_PERSISTENCE 0x61
#define BH1745_REG_THRESHOLD_LOW 0x64
#define BH1745_REG_THRESHOLD_HIGH 0x62
#define BH1745_REG_MANUFACTURER 0x92

#define BH1745_CHIP_ID 0b001011
#define BH1745_MANUFACTURER 0xe0

namespace pimoroni {
    typedef struct  {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t c;
    } rgbc_t;

    class BH1745 {
        public:
            static const uint8_t DEFAULT_I2C_ADDRESS      = 0x38;
            static const uint