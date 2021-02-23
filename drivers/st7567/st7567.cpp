#include "st7567.hpp"
#include <math.h>


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

namespace pimoroni {

  enum reg : uint8_t {
    DISPOFF   = 0xAE,//
    DISPON    = 0xAF,//
    SETSTARTLINE = 0x40,//
    STARTLINE_MASK = 0x3f,//
    REG_RATIO = 0x20,//
    SETPAGESTART = 0xb0,//
    PAGESTART_MASK = 0x07,//
    SETCOLL = 0x00,  //       # 0x00-0x0f: Set lower column address */
    COLL_MASK = 0x0f,//
    SETCOLH = 0x10,         //# 0x10-0x1f: Set higher colum