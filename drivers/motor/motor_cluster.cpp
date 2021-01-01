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
    delete[] config