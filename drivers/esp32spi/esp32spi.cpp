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
    GET_HOST_BY_NAME  