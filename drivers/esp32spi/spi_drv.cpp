#include "spi_drv.hpp"

namespace pimoroni {

  void SpiDrv::init() {
    spi_init(spi, 8000000);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(cs);
    gpio_set_dir(cs, GPIO_OUT);
    gpio_put(cs, true);

    gpio_init(gpio0);
    gpio_set_dir(gpio0, GPIO_OUT);

    gpio_init(resetn);
    gpio_set_dir(resetn, GPIO_OUT);
  }

  void SpiDrv::reset() {
    gpio_put(gpio0, true);
    gpio_put(cs, true);
    gpio_put(resetn, false);
    sleep_ms(10);
    gpio_put(resetn, true);
    sleep_ms(750);
  }

  bool SpiDrv::available() {
    return gpio_get(gpio0);
  }

  void SpiDrv::esp_select() {
    gpio_put(cs, false);
  }
    
  void SpiDrv::esp_deselect() {
    gpio_put(cs, true);
  }

  bool SpiDrv::get_esp_ready() {
    return !gpio_get(ack);
  }

  bool SpiDrv::get_esp_ack() {
    return gpio_get(ack);
  }

  bool SpiDrv::wait_for_esp_ack(uint32_t timeout_ms) {
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
	  while(!get_esp_ack()) {
      tight_loop_contents();
      if (absolute_time_diff_us(get_absolute_time(),  timeout) <= 0) {
        return false;
      }
    }
    return true;
  }

  bool SpiDrv::wait_for_esp_ready(uint32_t timeout_ms) {
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
    while(!get_esp_ready()) {
      tight_loop_contents();
      if (absolute_time_diff_us(get_absolute_time(),  timeout) <= 0) {
        return false;
      }
    }
    return true;
  }

  bool SpiDrv::wait_for_esp_select(uint32_t timeout_ms) {
    if(!wait_for_esp_ready(timeout_ms)) {
      return false;
    }
    esp_select();
    if(!wait_for_esp_ack(timeout_ms)) {
      esp_deselect();
      return false;
    }
    return true;
  }

  int SpiDrv::wait_for_byte(uint8_t wait_byte) {
    int timeout = BYTE_TIMEOUT;
    uint8_t byte_read = 0;
    do{
      byte_read = read_byte(); //get data byte
      if (byte_read == ERR_CMD) {
        WARN("Err cmd received\n");
        return -1;
      }
    } while((timeout-- > 0) && (byte_read != wait_byte));
    return (byte_read == wait_byte);
  }
    
  bool SpiDrv::read_and_check_byte(uint8_t check_byte, uint8_t *byte_out) {
    get_param(byte_out);
    return (*byte_out == check_byte);
  }

  uint8_t SpiDrv::read_byte() {
    uint8_t byte_read = 0;
    get_param(&byte_read);
    return byte_read;
  }

  bool SpiDrv::wait_response_params(uint8_t cmd, uint8_t num_param, outParam *params_out) {
    uint8_t data = 0;
    int i = 0;

    IF_CHECK_START_CMD() {
      CHECK_DATA(cmd | REPLY_FLAG, data){};

      uint8_t num_param_read = read_byte();
      if(num_param_read != 0) {        
        for(i = 0; i < num_param_read; ++i) {
          params_out[i].param_len = read_param_len8();
          spi_read_blocking(spi, DUMMY_DATA, params_out[i].param, params_out[i].param_len);
        }
      }
  