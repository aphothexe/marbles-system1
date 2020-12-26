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
  }

  uint16_t lookup::value(uint8_t index) {
    return lut[index];
  }

  pimoroni::lookup LTR559::lookup_led_current({5, 10, 20, 50, 100});
  pimoroni::lookup LTR559::lookup_led_duty_cycle({25, 50, 75, 100});
  pimoroni::lookup LTR559::lookup_led_pulse_freq({30, 40, 50, 60, 70, 80, 90, 100});
  pimoroni::lookup LTR559::lookup_proximity_meas_rate({10, 50, 70, 100, 200, 500, 1000, 2000});
  pimoroni::lookup LTR559::lookup_light_integration_time({100, 50, 200, 400, 150, 250, 300, 350});
  pimoroni::lookup LTR559::lookup_light_repeat_rate({50, 100, 200, 500, 1000, 2000});
  pimoroni::lookup LTR559::lookup_light_gain({1, 2, 4, 8, 0, 0, 48, 96});
  
  bool LTR559::init() {
    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    reset();
    interrupts(true, true);

    // 50mA, 100% duty cycle, 30Hz, 1 pulse
    proximity_led(50, 100, 30, 1);

    // enabled, gain 4x
    light_control(true, 4);

    // enabled, saturation indicator enabled
    proximity_control(true, true);

    // 100ms measurement rate
    proximity_measurement_rate(100);

    // 50ms integration time and repeat rate
    light_measurement_rate(50, 50);

    light_threshold(0xFFFF, 0x0000);
    proximity_threshold(0x7FFF, 0x7FFF);
    proximity_offset(0);

    return true;
  }

  void LTR559::reset() {
    i2c->set_bits(address, LTR559_ALS_CONTROL, LTR559_ALS_CONTROL_SW_RESET_BIT);

    while(i2c->get_bits(address, LTR559_ALS_CONTROL, LTR559_ALS_CONTROL_SW_RESET_BIT)) {
      sleep_ms(100);
    }
  }

  i2c_inst_t* LTR559::get_i2c() const {
    return i2c->get_i2c();
  }

  int LTR559::get_address() const {
    return address;
  }

  int LTR559::get_sda() const {
    return i2c->get_sda();
  }

  int LTR559::get_scl() const {
    return i2c->get_sda();
  }

  int LTR559::get_int() const {
    return interrupt;
  }


  uint8_t LTR559::part_id() {
    uint8_t part_id;
    i2c->read_bytes(address, LTR559_PART_ID, &part_id, 1);
    return (part_id >> LTR559_PART_ID_PART_NUMBER_SHIFT) & LTR559_PART_ID_PART_NUMBER_MASK;
  }

  uint8_t LTR559::revision_id() {
    uint8_t revision_id;
    i2c->read_bytes(address, LTR559_PART_ID, &revision_id, 1);
    return revision_id & LTR559_PART_ID_REVISION_MASK;
  }

  uint8_t LTR559::manufacturer_id() {
    uint8_t manufacturer;
    i2c->read_bytes(address, LTR559_MANUFACTURER_ID, &manufacturer, 1);
    return manufacturer;
  }

  bool LTR559::get_reading() {
    bool has_updated = false;
    uint8_t status;
    i2c->read_bytes(address, LTR559_ALS_PS_STATUS, &status, 1);
    bool als_int = (status >> LTR559_ALS_PS_STATUS_ALS_INTERRUPT_BIT) & 0b1;
    bool ps_int = (status >> LTR559_ALS_PS_STATUS_PS_INTERRUPT_BIT) & 0b1;
    bool als_data = (status >> LTR559_ALS_PS_STATUS_ALS_DATA_BIT) & 0b1;
    bool ps_data = (status >> LTR559_ALS_PS_STATUS_PS_DATA_BIT) & 0b1;

    if(ps_int || ps_data) {
      has_updated = true;
      uint16_t ps0;
      i2c->read_bytes(address, LTR559_PS_DATA, (uint8_t *)&ps0, 2);
      ps0 &= LTR559_PS_DATA_MASK;

      data.proximity = ps0;
    }

    if(als_int || als_data) {
      has_updated = true;
      uint16_t als[2];
      i2c->read_bytes(address, LTR559_ALS_DATA_CH1, (uint8_t *)&als, 4);
      data.als0 = als[1];
      data.als1 = als[0];
      data.gain = this->lookup_light_gain.value((status >> LTR559_ALS_PS_STATUS_ALS_GAIN_SHIFT) & LTR559_ALS_PS_STATUS_ALS_GAIN_MASK);

      data.ratio = 101.0f;

      if((uint32_t)data.als0 + data.als1 > 0) {
        data.ratio = (float)data.als1 * 100.0f / ((float)data.als1 + data.als0);
      }

      uint8_t ch_idx = 3;
      if(this->data.ratio < 45)
        ch_idx = 0;
      else if(data.ratio < 64)
        ch_idx = 1;
      else if (data.ratio < 85)
        ch_idx = 2;

      float lux = ((int32_t)data.als0 * ch0_c[ch_idx]) - ((int32_t)data.als1 * ch1_c[ch_idx]);
      lux /= (float)this->data.integration_time / 100.0f;
      lux /= (f