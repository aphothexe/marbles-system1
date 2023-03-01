#include "hardware/gpio.h"
#include "wakeup.config.hpp"

extern uint32_t runtime_wakeup_gpio_state;

namespace {
    struct Wakeup {
        public:
            uint8_t shift_register_state = 0b0;

            Wakeup() {
                // Assert wakeup pins (indicator LEDs, VSYS hold etc)
                //gpio_init_mask(WAKEUP_PIN_MASK);
                //gpio_set_dir_masked(WAKEUP_PIN_MASK, WAKEUP_PIN_DIR);
                //gpio_put_masked(WAKEUP_PIN_MASK, WAKEUP_PIN_VALUE);

#if WAKEUP_HAS_RTC==1
                // Set up RTC I2C pins and send reset command
                i2c_init(WAKEUP_RTC_I2C_INST, 100000);
                gpio_init(WAKEUP_RTC_SDA);
                gpio_init(WAKEUP_RTC_SCL);
                gpio_set_function(WAKEUP_RTC_SDA, GPIO_FUNC_I2C); gpio_pull_up(WAKEUP_RTC_SDA);
                gpio_set_function(WAKEUP_RTC_SCL, GPIO_FUNC_I2C); gpio_pull_up(WAKEUP_RTC_SCL);

                // Turn off CLOCK_OUT by writing 0b111 to CON