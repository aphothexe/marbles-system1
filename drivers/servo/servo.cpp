#include "servo.hpp"
#include "hardware/clocks.h"
#include "pwm.hpp"

namespace servo {
  Servo::Servo(uint pin, CalibrationType type, float freq)
    : servo_pin(pin), state(type), pwm_frequency(freq) {
  }

  Servo::Servo(uint pin, const Calibration& calibration, float freq)
    : servo_pin(pin), state(calibration), pwm_frequency(freq) {
  }

  Servo::~Servo() {
    gpio_set_function(servo_pin, GPIO_FUNC_NULL);
  }

  bool Servo::init() {
    bool success = false;

    uint16_t period; uint16_t div16;
    if(pimoroni::calculate_pwm_factors(pwm_frequency, period, div16)) {
      pwm_period = period;

      pwm_cfg = pwm_get_default_config();

      // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
      pwm_config_set_wrap(&pwm_cfg, pwm_period - 1);

      // Apply the divider
      pwm_config_set_clkdiv(&pwm_cfg, (float)div16 / 16.0f); // There's no 'pwm_config_set_clkdiv_int_frac' for some reason...

      pwm_init(pwm_gpio_to_slice_num(servo_pin), &pwm_cfg, true);
      gpio_set_function(servo_pin, GPIO_FUNC_PWM);

      pwm_set_gpio_level(servo_pin, 0);

      success = true;
    }
    return success;
  }

  uint Servo::