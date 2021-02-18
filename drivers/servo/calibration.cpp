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
    apply_default_pairs(default_type);
  }

  Calibration::Calibration(const Calibration &other)
    : calibration(nullptr), calibration_size(0), limit_lower(other.limit_lower), limit_upper(other.limit_upper) {
    uint size = other.size();
    apply_blank_pairs(size);
    for(uint i = 0; i < size; i++) {
      calibration[i] = other.calibration[i];
    }
  }

  Calibration::~Calibration() {
    if(calibration != nullptr) {
      delete[] calibration;
      calibration = nullptr;
    }
  }

  Calibration &Calibration::operator=(const Calibration &other) {
    uint size = other.size();
    apply_blank_pairs(size);
    for(uint i = 0; i < size; i++) {
      calibration[i] = other.calibration[i];
    }
    limit_lower = other.limit_lower;
    limit_upper = other.limit_upper;

    return *this;
  }

  Calibration::Pair &Calibration::operator[](uint8_t index) {
    assert(index < calibration_size);
    return calibration[index];
  }

  const Calibration::Pair &Calibration::operator[](uint8_t index) const {
    assert(index < calibration_size);
    return calibration[index];
  }

  void Calibration::apply_blank_pairs(uint size) {
    if(calibration != nullptr) {
      delete[] calibration;
    }

    if(size > 0) {
      calibration = new Pair[size];
      calibration_size = size;
    }
    else {
      calibration = nullptr;
      calibration_size = 0;
    }
  }

  void Calibration::apply_two_pairs(float min_pulse, float max_pulse, float min_value, float max_value) {
    apply_blank_pairs(2);
    calibration[0] = Pair(min_pulse, min_value);
    calibration[1] = Pair(max_pulse, max_value);
  }

  void Calibration::apply_three_pairs(float min_pulse, float mid_pulse, float max_pulse, float min_value, float mid_value, float max_value) {
    apply_blank_pairs(3);
    calibration[0] = Pair(min_pulse, min_value);
    calibration[1] = Pair(mid_pulse, mid_value);
    calibration[2] = Pair(max_pulse, max_value);
  }

  void Calibration::apply_uniform_pairs(uint size, float min_pulse, float max_pulse, float min_value, float max_value) {
    apply_blank_pairs(size);
    if(size > 0) {
      float size_minus_one = (float)(size - 1);
      for(uint i = 0; i < size; i++) {
        float pulse = Calibration::map_float((float)i, 0.0f, size_minus_one, min_pulse, max_pulse);
        float value = Calibration::map_float((float)i, 0.0f, size_minus_one, min_value, max_value);
        calibration[i] = Pair(pulse, value);
      }
    }
  }

  void Calibration::apply_default_pairs(CalibrationType default_type) {
    switch(default_type) {
    default:
    case ANGULAR:
      apply_three_pairs(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE,
                       -90.0f,            0.0f,              +90.0f);
      break;
    case LINEAR:
      apply_two_pairs(DEFAULT_MIN_PULSE, DEFAULT_MAX_PULSE,
                     0.0f,              1.0f);
      break;
    case CONTINUOUS:
      apply_three_pairs(DEFAULT_MIN_PULSE, DEFAULT_MID_PULSE, DEFAULT_MAX_PULSE,
                       -1.0f,            0.0f,              +1.0f);
      break;
    }
  }

  uint Calibration::size() const {
    return calibration_size;
  }

  Calibration::Pair &Calibration::pair(uint8_t index) {
    assert(index < calibration_size);
    return calibration[index];
  }

  const Calibration::Pair &Calibration::pair(uint8_t index) const {
    assert(index < calibration_size);
    return calibration[index];
  }

  float Calibration::pulse(uint8_t index) const {
    return pair(index).pulse;
  }

  void Calibration::pulse(uint8_t index, float pulse) {
    pair(index).pulse = pulse;
  }

  float Calibration::value(uint8_t index) const {
    return pair(index).value;
  }

  void Calibration::value(uint8_t index, float value) {
    pair(index).value = value;
  }

  Calibration::Pair &Calibration::first() {
    assert(calibration_size > 0);
    return calibration[0];
  }

  const Calibration::Pair &Calibration::first() const {
    assert(calibration_size > 0);
    return calibration[0];
  }

  float Calibration::first_pulse() c