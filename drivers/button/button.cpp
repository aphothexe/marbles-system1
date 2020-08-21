#include "button.hpp"

namespace pimoroni {
    bool Button::raw() {
      if(polarity == Polarity::ACTIVE_LOW){
        return !gpio_get(pin);
      } else {
        return gpio_get(pin);
      }
    }

    bool Button::read() {
      auto time = millis();
      bool state = raw();
      bool changed = state != last_state;
      last_state = state;

    