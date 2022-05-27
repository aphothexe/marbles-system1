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
    HTTP_REQUEST_CONNECTION_FAILED,
    HTTP_REQUEST_RESPONSE_OVERFLOW,
    HTTP_REQUEST_NO_RESPONSE
};

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

/* Basic function to connect to a client IP:PORT and poll for an established connection */
bool connect(IPAddress host_address, uint16_t port, uint8_t client_sock, uint32_t timeout = 1000) {
    wireless.start_client(host_address, port, client_sock, TCP_MODE);

    uint32_t t_start = millis();

    while(millis() - t_start < timeout) {
        uint8_t state = wireless.get_client_state(client_sock);
        if(state == ESTABLISHED) return true;
        sleep_ms(100);
    }

    return false;
}

/* Basic DNS lookup */
IPAddress dns_lookup(std::string request_host) {
    IPAddress host_address(0, 0, 0, 0);
    printf("DNS lookup for: %s\n", request_host.c_str());
    if(!wireless.get_host_by_name(request_host.c_str(), host_address)) {
        printf("DNS lookup failed!\n");
    }
    return host_address;
}

/* This is pretty much the simplest HTTP request function I could get away with.
   It accepts a client socket ID, IP address, hostname, request path and handler function,
   and calls the handler with the status code (only 200 & 404 at the moment), head
   and body as `std::vector<std::string_view>` into the underlying buffer.
 */
HTTP_REQUEST_STATUS http_request(uint8_t client_sock, IPAddress host_address, uint16_t port, std::string request_host, std::string request_path, http_handler handler, uint32_t timeout = 1000) {
    if(!connect(host_address, port, client_sock)) {
        printf("Connection failed!\n");
        return HTTP_REQUEST_CONNECTION_FAILED;
    }

    // HTTP request to grab our API endpoint
    const std::string http_request = "GET " + request_path + " HTTP/1.1\r\n\
Host: " + request_host + "\r\n\
Connection: close\r\n\r\n";

    // Clear the response buffer
    memset(response_buf, 0, HTTP_RESPONSE_BUF_SIZE);

    wireless.send_data(client_sock, (const uint8_t *)http_request.data(), http_request.length());

    uint16_t response_length = 0;
    uint16_t avail_length = 0;
    uint32_t t_start = millis();

    // Keep receiving data until our designated timeout
    // There's no guarantee that `wireless.avail_data` will have the *whole* response in one shot
    // and I *really* don't want to parse the HTTP response for a `Content-Length` header.
    while(millis() - t_start < timeout) {
        sleep_ms(50);
        avail_length = wireless.avail_data(client_sock);
        if(avail_length > 0) break;
    }

    // Read the full response
    // Sometimes the bytes read is less than the bytes we request, so loop until we get the data we expect
    while(response_length < avail_length) {
        uint16_t read_length = avail_length; // Request the full buffer
