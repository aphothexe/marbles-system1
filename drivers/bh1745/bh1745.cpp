#include "bh1745.hpp"
#include <algorithm>

namespace pimoroni {
    bool BH1745::init() {
        reset();

        if (this->get_chip_id() != BH1745_CHIP_ID || this->get_manufacturer() != BH1745_MANUFACTURER) {
            return false;
        }

        reset();
        i2c->clear_bits(address, BH1745_REG_SYSTEM_CONTROL, 6);    // Clear INT reset bit
        set_measurement_time_ms(640);
        i2c->set_bits(address, BH1745_REG_MODE_CONTROL2, 4);       // Enable RGBC
        i2c->set_bits(address, BH1745_REG_MODE_CONTROL3, 0, 0xff); // Turn on sensor
        set_threshold_high(0x0000);                                // Set threshold so int will always fire
        set_threshold_low(0xFFFF);                                 // this lets us turn on the LEDs with the int pin
        i2c->clear_bits(address, BH1745_REG_INTERRUPT, 4);         // Enable interrupt latch

        sleep_ms(320);

        return true;
    