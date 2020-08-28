#include "ip_address.hpp"

namespace pimoroni {

  IPAddress::IPAddress() {
    addr.dword = 0;
  }

  IPAddress::IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    addr.bytes[0] = first_octet;
    addr.bytes[1] = second_octet;
    addr.bytes[2] = third_octet;
    addr.bytes[3] = fourth_octet;
  }
    
  IPAddress::IPAddress(uint32_t address) {
    addr.dword = address;
  }

  IPAddress::IPAddress(const uint8_t *address)