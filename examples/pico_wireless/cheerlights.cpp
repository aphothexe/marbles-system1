#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico_wireless.hpp"
#include "secrets.h"

#include <vector>
#include <algorithm>

#define HTTP_PORT 80
#define HTTP_REQUEST_BUF_SIZE 2048

#define DNS_CLOUDFLARE IPAddress(1, 1, 1, 1)
#define DNS_GOOGLE IPAddress(8, 8, 8, 8)
#define USE_DNS DNS_CLOUDFLARE

#define HTTP_REQUEST_DELAY 30  // Seconds between requests
#define HTTP_REQUEST_HOST "api.thingspeak.com"
#define HTTP_REQUEST_PATH "/channels/1417/field/2/last.txt"
#define HTTP_RESPONSE_BUF_SIZE 1024

using namespace pimoroni;

PicoWireless wireless;
uint8_t r, g, b;
uint8_t response_buf[HTTP_RESPONSE_BUF_SIZE];
typedef void(*http_handler)(unsigned int status_code, std::vector<std::string_view> response_head, std::vector<std::string_view> esponse_body);

enum HTTP_REQUEST_STATUS {
    HTTP_REQUEST_OK = 0,
    HTTP_REQUEST_TIMEOUT,
    HTTP_REQUEST_RESPONSE_INVALID,
    HTTP_REQUEST_RESPONSE_UNHANDLED,
    HTT