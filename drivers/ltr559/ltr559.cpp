#include "ltr559.hpp"
#include <algorithm>

namespace pimoroni {
  lookup::lookup(std::initializer_list<uint16_t> values) : lut(values) {
  }

  uint8_t lookup::index(uint16_t value) {
    auto it = find(lut.begin(), lut.end(), value);

    if(it == lut.end())
      return 0;

    return it - lut.begin();