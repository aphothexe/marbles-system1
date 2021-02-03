#include "calibration.hpp"

namespace servo {
  Calibration::Pair::Pair()
  : pulse(0.0f), value(0.0f) {
  }

  Calibration::Pair::Pair(float pulse, float value)
  : pulse(pulse), value(value) {
  }

  Calibration::Calibration()
    : calibration(nullptr), calibration_size(0), limit_lower(true), limit_upper(true) {
  }

  Calibration::Calibration(CalibrationType default_type)
    : Calibration() {
    apply_defaul