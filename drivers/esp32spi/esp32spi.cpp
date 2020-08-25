#include "esp32spi.hpp"

namespace pimoroni {


  enum cmd {
    // 0x10 -> 0x1f
    SET_NET                 = 0x10,
    SET_PASSPHRASE          = 0x11,
    SET_KEY                 = 0x12,
    //NULL
    SET_IP_CONFIG           = 0x14,
    SET_DNS_CONFIG          = 0x15,
    SET_HOSTNAME            = 0x16,
    SET_POWER_MODE          = 0x17,
    SET_AP_NET              = 0x18,
    SET_AP_PASSPHRASE       = 0x19,
    SET_DEBUG               = 0x1a,
    GET_TEMPERATURE         = 0x1b,
    //NULL, NULL, NULL, NULL,

    // 0x20 -> 0x2f
    GET_CONN_STATUS         = 0x20,
    GET_IP_ADDR             = 0x21,
    GET_MAC_ADDR            = 0x22,
    GET_CURR_SSID           = 0x23,
    GET_CURR_BSSID          = 0x24,
    GET_CURR_RSSI           = 0x25,
    GET_CURR_ENCT           = 0x26,
    SCAN_NETWORKS           = 0x27,
    START_SERVER_TCP        = 0x28,
    GET_STATE_TCP           = 0x29,
    DATA_SENT_TCP           = 0x2a,
    AVAIL_DATA_TCP          = 0x2b,
    GET_DATA_TCP            = 0x2c,
    START_CLIENT_TCP        = 0x2d,
    STOP_CLIENT_TCP         = 0x2e,
    GET_CLIENT_STATE_TCP    = 0x2f,

    // 0x30 -> 0x3f
    DISCONNECT              = 0x30,
    //NULL,
    GET_IDX_RSSI            = 0x32,
    GET_IDX_ENCT            = 0x33,
    REQ_HOST_BY_NAME        = 0x34,
    GET_HOST_BY_NAME        = 0x35,
    START_SCAN_NETWORKS     = 0x36,
    GET_FW_VERSION          = 0x37,
    //NULL,
    SEND_DATA_UDP           = 0x39,
    GET_REMOTE_DATA         = 0x3a,
    GET_TIME                = 0x3b,
    GET_IDX_BSSID           = 0x3c,
    GET_IDX_CHANNEL         = 0x3d,
    PING                    = 0x3e,
    GET_SOCKET              = 0x3f,

    // 0x40 -> 0x4f
    SET_CLIENT_CERT         = 0x40,         //NOTE No matching function
    SET_CERT_KEY            = 0x41,         //NOTE No matching function
    //NULL, NULL,
    SEND_DATA_TCP           = 0x44,
    GET_DATABUF_TCP         = 0x45,
    INSERT_DATABUF          = 0x46,
    //NULL, NULL, NULL,
    WPA2_ENT_SET_IDENTITY   = 0x4a,
    WPA2_ENT_SET_USERNAME   = 0x4b,
    WPA2_ENT_SET_PASSWORD   = 0x4c,
    WPA2_ENT_SET_CA_CERT    = 0x4d,         //NOTE Not functional in Nina FW
    WPA2_ENT_SET_CERT_KEY   = 0x4e,         //NOTE Not functional in Nina FW
    WPA2_ENT_ENABLE         = 0x4f,

    // 0x50 -> 0x5f
    SET_PIN_MODE            = 0x50,
    SET_DIGITAL_WRITE       = 0x51,
    SET_ANALOG_WRITE        = 0x52,
    SET_DIGITAL_READ        = 0x53,
    SET_ANALOG_READ         = 0x54,

    SET_WAKE_PIN            = 0x55,
    SET_LIGHT_SLEEP         = 0x56,
    SET_DEEP_SLEEP          = 0x57,
  };

  bool Esp32Spi::init() {
    driver.init();
    driver.reset();

    return true;
  }

  bool Esp32Spi::get_network_data(uint8_t *ip_out, uint8_t *mask_out, uint8_t *gwip_out) {
    SpiDrv::outParam params_out[SpiDrv::PARAM_NUMS_3] = { {0, ip_out},
                                                    {0, mask_out},
                                                    {0, gwip_out} };

    return driver.send_command(GET_IP_ADDR, params_out, SpiDrv::PARAM_NUMS_3);
  }

  bool Esp32Spi::get_remote_data(uint8_t sock, uint8_t *ip_out, uint8_t *port_out) {
    SpiDrv::outParam params_out[SpiDrv::PARAM_NUMS_2] = { {0, ip_out},
                                                    {0, port_out} };

    return driver.send_command(GET_REMOTE_DATA, params_out, SpiDrv::PARAM_NUMS_2);
  }

  int8_t Esp32Spi::wifi_set_network(const std::string ssid) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_NET, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_NET\n");
    }
    return (data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
  }

  int8_t Esp32Spi::wifi_set_passphrase(const std::string ssid, const std::string passphrase) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid),
      SpiDrv::build_param(&passphrase)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_PASSPHRASE, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_PASSPHRASE\n");
    }
    return data;
  }

  int8_t Esp32Spi::wifi_set_key(const std::string ssid, uint8_t key_idx, const std::string key) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&ssid),
      SpiDrv::build_param(&key_idx),
      SpiDrv::build_param(&key)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_KEY, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_KEY\n");
    }
    return data;
  }

  void Esp32Spi::config(uint8_t valid_params, uint32_t local_ip, uint32_t gateway, uint32_t subnet) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&valid_params),
      SpiDrv::build_param(&local_ip),
      SpiDrv::build_param(&gateway),
      SpiDrv::build_param(&subnet)
    };

    uint8_t data = 0;
    uint16_t data_len = 0;
    if (!driver.send_command(SET_IP_CONFIG, params, PARAM_COUNT(params), &data, &data_len)) {
      WARN("Error:SET_IP_CONFIG\n");
    }
  }

  void Esp32Spi::set_dns(uint8_t valid_params, uint32_t dns_server1, uint32_t dns_server2) {
    SpiDrv::inParam params[] = {
      SpiDrv::build_param(&valid_params),
      SpiDrv::build_param(&dns_server1),
      SpiDrv::build_param(&dns_server2)
    };

    uint8_t data = 0;
    uint16_t data_l