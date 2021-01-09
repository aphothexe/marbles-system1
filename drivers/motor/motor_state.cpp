#include "motor_state.hpp"
#include "common/pimoroni_common.hpp"
#include "float.h"
#include "math.h"

namespace motor {
  MotorState::MotorState()
    : motor_speed(0.0f), last_enabled_duty(0.0f), enabled(false)
    , motor_direction(NORMAL_DIR), motor_scale(DEFAULT_SPEED_SCALE)
    , motor_zeropoint(DEFAULT_ZEROPOINT), motor_deadzone(DEFAULT_DEADZONE) {
  }

  MotorState::MotorState(Direction direction, float speed_scale, float zeropoint, float deadzone)
    : motor_speed(0.0f), last_enabled_duty(0.0f), enabled(false)
    , motor_direction(direction) , motor_scale(MAX(speed_scale, FLT_EPSILON))
    , motor_zeropoint(CLAMP(zeropoint, 0.0f, 1.0f - FLT_EPSILON)), motor_deadzone(CLAMP(deadzone, 0.0f, 1.0f)) {
  }

  float MotorState::enable_with_return() {
    enabled = true;
    return get_deadzoned_duty();
  }

  float MotorState::disable_with_return() {
    enabled = false;
    return NAN;
  }

  bool MotorState::is_enabled() const {
    return enabled;
  }

  float MotorState::get_duty() const {
    return (motor_direction == NORMAL_DIR) ? last_enabled_duty : 0.0f - last_enabled_duty;
  }

  float MotorState::get_deadzoned_duty() const {
    float duty = 0.0f;
    if((last_enabled_duty <= 0.0f - motor_deadzone) || (last_enabled_duty >= motor_deadzone)) {
      duty = last_enabled_duty;
    }

    if(enabled)
      return duty;
    else
      return NAN;
  }

  float MotorState::set_duty_with_return(float duty) {
    // Invert provided speed if the motor direction is reversed
    if(motor_direction == REVERSED_DIR)
      duty = 0.0f - duty;

    // Clamp the duty between the hard limits
    last_enabled_duty = CLAMP(duty, -1.0f, 1.0f);

    // Calculate the corresponding speed
    motor_speed = duty_to_speed(last_enabled_duty, motor_zeropoint, motor_scale);

    return enable_with_return();
  }

  float MotorState::get_speed() const {
    return (motor_direction == NORMAL_DIR) ? motor_speed : 0.0f - motor_spee