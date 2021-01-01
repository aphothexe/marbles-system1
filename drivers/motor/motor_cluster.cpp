#include "motor_cluster.hpp"
#include "pwm.hpp"
#include <cstdio>
#include "math.h"

#define POS_MOTOR(motor)  (PWMCluster::channel_from_pair(motor))
#define NEG_MOTOR(motor)  (PWMCluster::channel_from_pair(motor) + 1)

namespace motor {
  MotorCluster::MotorCluster(PIO pio, uint sm, uint pin_base, uint pin_pair_count, Direction direction,
                             float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode,
                             bool auto_phase)
    : pwms(pio, sm, pin_base, (pin_pair_count * 2), false), pwm_frequency(freq) {
    create_motor_states(direction, speed_scale, zeropoint, deadzone, mode, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, const pin_pair *pin_pairs, uint32_t length, Direction direction,
                             float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode,
                             bool auto_phase)
    : pwms(pio, sm, pin_pairs, length, false), pwm_frequency(freq) {
    create_motor_states(direction, speed_scale, zeropoint, deadzone, mode, auto_phase);
  }

  MotorCluster::MotorCluster(PIO pio, uint sm, std::initializer_list<pin_pair> pin_pairs, Direction direction,
                             float speed_scale, float zeropoint, float deadzone, float freq, DecayMode mode,
                             bool auto_phase)
    : pwms(pio, sm, pin_pairs, false), pwm_frequency(freq) {
    create_motor_states(direction, speed_scale, zeropoint, deadzone, mode, auto_phase);
  }

  MotorCluster::~MotorCluster() {
    delete[] states;
    delete[] configs;
  }

  bool MotorCluster::init() {
    bool success = false;

    if(pwms.init()) {
      // Calculate a suitable pwm wrap period for this frequency
      uint32_t period; uint32_t div256;
      if(pimoroni::PWMCluster::calculate_pwm_factors(pwm_frequency, period, div256)) {
        pwm_period = period;

        // Update the pwm before setting the new wrap
        uint8_t motor_count = pwms.get_chan_pair_count();
        for(uint8_t motor = 0; motor < motor_count; motor++) {
          pwms.set_chan_level(POS_MOTOR(motor), 0, false);
          pwms.set_chan_level(NEG_MOTOR(motor), 0, false);
          pwms.set_chan_offset(POS_MOTOR(motor), (uint32_t)(configs[motor].phase * (float)pwm_period), false);
          pwms.set_chan_offset(NEG_MOTOR(motor), (uint32_t)(configs[motor].phase * (float)pwm_period), false);
        }

        // Set the new wrap (should be 1 less than the period to get full 0 to 100%)
        pwms.set_wrap(pwm_period, true); // NOTE Minus 1 not needed here. Maybe should change Wrap behaviour so it is needed, for consistency with hardware pwm?

        // Apply the new divider
        // This is done after loading new PWM speeds to avoid a lockup condition
        uint8_t div = div256 >> 8;
        uint8_t mod = div256 % 256;
        pwms.set_clkdiv_int_frac(div, mod);

        success = true;
      }
    }

    return success;
  }

  uint8_t MotorCluster::count() const {
    return pwms.get_chan_pair_count();
  }

  pin_pair MotorCluster::pins(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return pwms.get_chan_pin_pair(motor);
  }

  void MotorCluster::enable(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].enable_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::enable(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      enable(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::enable(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::enable_all(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      enable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable(uint8_t motor, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].disable_with_return();
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::disable(const uint8_t *motors, uint8_t length, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      disable(motors[i], false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable(std::initializer_list<uint8_t> motors, bool load) {
    for(auto motor : motors) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  void MotorCluster::disable_all(bool load) {
    uint8_t motor_count = pwms.get_chan_pair_count();
    for(uint8_t motor = 0; motor < motor_count; motor++) {
      disable(motor, false);
    }
    if(load)
      pwms.load_pwm();
  }

  bool MotorCluster::is_enabled(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].is_enabled();
  }

  float MotorCluster::duty(uint8_t motor) const {
    assert(motor < pwms.get_chan_pair_count());
    return states[motor].get_duty();
  }

  void MotorCluster::duty(uint8_t motor, float duty, bool load) {
    assert(motor < pwms.get_chan_pair_count());
    float new_duty = states[motor].set_duty_with_return(duty);
    apply_duty(motor, new_duty, configs[motor].mode, load);
  }

  void MotorCluster::duty(const uint8_t *motors, uint8_t length, float duty, bool load) {
    assert(motors != nullptr);
    for(uint8_t i = 0; i < length; i++) {
      this->duty(motors[i], d