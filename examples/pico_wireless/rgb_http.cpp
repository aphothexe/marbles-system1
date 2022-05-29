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

using namespace pimoroni;

PicoWireless wireless;
uint8_t r, g, b;
uint8_t request_buf[HTTP_REQUEST_BUF_SIZE];
const std::string response_501 = "HTTP/1.1 501 Not Implemented\nContent-Length: 19\n\n501 Not Implemented";

enum http_request_method_t {
  GET,
  POST
};
typedef std::string_view(*http_request_handler)(http_request_method_t method, std::string_view path, std::vector<std::string_view> request_head, std::vector<std::string_view> request_body);

std::vector<std::string_view> split(std::string_view str, std::string delim="\r\n") {
  std::vector<std::string_view> result;
  size_t offset = 0;
  while (offset < str.size()) {
    const auto pos = str.find_first_of(delim, offset);
    // Emit an empty view even if two adjacent delimiters are found
    // this ensurs the HTTP "blank line" start of content is found
    result.emplace_back(str.substr(offset, pos - offset));
    if (pos == std::string_view::npos) break;
    offset = pos + delim.length();
  }
  return result;
}

uint32_t millis() {
  return to_us_since_boot(get_absolute_time()) / 1000;
}

bool wifi_connect(std::string network, std::string password, IPAddress dns_server, uint32_t timeout=10000) {
  printf("Connecting to %s...\n", network.c_str());
  wireless.wifi_set_passphrase(network, password);

  uint32_t t_start = millis();

  while(millis() - t_start < timeout) {
    if(wireless.get_connection_status() == WL_CONNECTED) {
      printf("Connected!\n");
      wireless.set_dns(1, dns_server, 0);
      return true;
    }
    wireless.set_led(255, 0, 0);
    sleep_ms(500);
    wireless.set_led(0, 0, 0);
    sleep_ms(500);
    printf("...\n");
  }

  return false;
}

int socket_accept(int8_t server_sock) {
  uint8_t new_sock = wireless.avail_server(server_sock);
  if(new_sock != server_sock
  && new_sock != 255) {
     return new_sock;
  }
  return -1;
}

uint8_t start_server(uint16_t http_port) {
  printf("Starting server...\n");
  // Get a socket for our server
  uint8_t server_sock = wireless.get_socket();
  wireless.start_server(http_port, server_sock);
  while(1){
    uint8_t state = wireless.get_server_state(server_sock);
    if(state == LISTEN) break;
  }
  IPAddress ip;
  wireless.get_ip_address(ip);
  printf("Server listening on %s:%i\n", ip.to_string().c_str(), http_port);
  return server_sock;
}

bool poll_http_requests(uint8_t server_sock, http_request_handler request_handler, uint32_t timeout = 1000) {
  uint16_t request_length = 0;
  uint16_t avail_length = 0;
  uint32_t t_start = millis();

  // Clear the request buffer
  memset(request_buf, 0, HTTP_REQUEST_BUF_SIZE);

  // Try to accept an incoming connection
  int client_sock = socket_accept(server_sock);
  if(client_sock == -1) return false;

  printf("Got client: %d\n", client_sock);

  // Any data? This *usually* seems to be available immediately,
  // so we can dispense with any ongoing polling and hope for the best.
  avail_length = wireless.avail_data(client_sock);
  if(avail_length == 0) return false;

  printf("Got request: %i bytes\n",  avail_length);

  // Read the full response
  // Sometimes the bytes read is less than the bytes we request, so loop until we get the data we expect
  while(request_length < avail_length) {
    uint16_t read_length = avail_length; // Request the full buffer
    wireless.get_data_buf(client_sock, request_buf + request_length, &read_length);
    request_length += read_length; // Increment the response_length by the amount we actually read

    // Also check for timeout