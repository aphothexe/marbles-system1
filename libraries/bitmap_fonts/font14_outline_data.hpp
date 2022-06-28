#pragma once

#include "bitmap_fonts.hpp"

const bitmap::font_t font14_outline {
  .height = 14,
  .max_width = 10,
  .widths = {
    5, 3, 5,10, 7,10,10, 3, 5, 5, 6, 7, 4, 7, 3, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 4, 5, 8, 5, 7,
    8, 7, 6, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6,10, 7, 8,
    6, 9, 6, 6, 7, 7, 7, 9, 7, 7, 7, 5, 7, 5, 7, 8,
    5, 8, 6, 5, 6, 6, 5, 7, 6, 3, 5, 6, 5, 9, 6, 7,
    6, 7, 5, 5, 6, 6, 7, 9, 7, 7, 7, 5, 3, 5, 8, 5,
//  Extra
//  Æ  Þ  ß  æ   þ  £  ¥  ©  °
    9, 6, 6, 10, 6, 6, 6, 6, 5
  },
  .data = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //  
    0x0f, 0xfc, 0x0a, 0x04, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // !
    0x00, 0x7c, 0x00, 0x44, 0x00, 0x7c, 0x00, 0x44, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // "
    0x03, 0xf0, 0x02, 0xd0, 0x0e, 0xdc, 0x08, 0x04, 0x0e, 0xdc, 0x0e, 0xdc, 0x08, 0x04, 0x0e, 0xdc, 0x02, 0xd0, 0x03, 0xf0,  // //
    0x0e, 0xf8, 0x0b, 0x8c, 0x1b, 0x76, 0x10, 0x02, 0x1b, 0x76, 0x0c, 0xd4, 0x07, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // $
    0x00, 0x38, 0x00, 0x6c, 0x0f, 0x54, 0x09, 0xec, 0x0e, 0x78, 0x07, 0x9c, 0x0d, 0xe4, 0x0a, 0xbc, 0x0d, 0x80, 0x07, 0x00,  // %
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // &
    0x00, 0x7c, 0x00, 0x44, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // '
    0x03, 0xf0, 0x0e, 0x1c, 0x19, 0xe6, 0x17, 0x3a, 0x1c, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // (
    0x1c, 0x0e, 0x17, 0x3a, 0x19, 0xe6, 0x0e, 0x1c, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // )
    0x00, 0xfc, 0x00, 0xb4, 0x00, 0xcc, 0x00, 0xcc, 0x00, 0xb4, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // *
    0x01, 0xc0, 0x01, 0x40, 0x07, 0x70, 0x04, 0x10, 0x07, 0x70, 0x01, 0x40, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // +
    0x1c, 0x00, 0x17, 0x00, 0x19, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ,
    0x01, 0xc0, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // -
    0x0e, 0x00, 0x0a, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // .
    0x1e, 0x00, 0x13, 0x80, 0x1c, 0xe0, 0x07, 0x38, 0x01, 0xce, 0x00, 0x72, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // /

    0x07, 0xf8, 0x0c, 0x0c, 0x0b, 0xf4, 0x0a, 0x14, 0x0b, 0xf4, 0x0c, 0x0c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0
    0x0e, 0x70, 0x0a, 0x58, 0x0b, 0xec, 0x08, 0x04, 0x0b, 0xfc, 0x0a, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 1
    0x0e, 0x38, 0x0b, 0x2c, 0x09, 0xb4, 0x0a, 0xd4, 0x0b, 0x74, 0x0b, 0x8c, 0x0e, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 2
    0x07, 0x38, 0x0d, 0x2c, 0x0b, 0x34, 0x0b, 0xf4, 0x0b, 0x34, 0x0c, 0xcc, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 3
    0x03, 0xc0, 0x02, 0x70, 0x02, 0x98, 0x0e, 0xec, 0x08, 0x04, 0x0e, 0xfc, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 4
    0x0e, 0xfc, 0x0a, 0x84, 0x0a, 0xb4, 0x0a, 0xb4, 0x0b, 0xb4, 0x0c, 0x74, 0x07, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 5
    0x07, 0xf8, 0x0c, 0x0c, 0x0b, 0xb4, 0x0a, 0xb4, 0x0b, 0xb4, 0x0c, 0x74, 0x07, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 6
    0x00, 0x1c, 0x00, 0x14, 0x0f, 0x94, 0x08, 0xf4, 0x0f, 0x34, 0x01, 0xc4, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 7
    0x07, 0xf8, 0x0c, 0x4c, 0x0b, 0xb4, 0x0a, 0xb4, 0x0b, 0xb4, 0x0c, 0x4c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 8
    0x0e, 0xf8, 0x0b, 0x8c, 0x0b, 0x74, 0x0b, 0x54, 0x0b, 0x74, 0x0c, 0x0c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 9
    0x0e, 0x1c, 0x0a, 0x14, 0x0e, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // :
    0x1c, 0x00, 0x17, 0x1c, 0x19, 0x14, 0x0f, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ;
    0x03, 0x80, 0x06, 0xc0, 0x0d, 0x60, 0x0b, 0xa0, 0x0e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // <
    0x0e, 0xe0, 0x0a, 0xa0, 0x0a, 0xa0, 0x0a, 0xa0, 0x0a, 0xa0, 0x0a, 0xa0, 0x0a, 0xa0, 0x0e, 0xe0, 0x00, 0x00, 0x00, 0x00,  // =
    0x0e, 0xe0, 0x0b, 0xa0, 0x0d, 0x60, 0x06, 0xc0, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // >
    0x00, 0x00, 0x00, 0x1c, 0x0f, 0xd4, 0x0a, 0x74, 0x0f, 0xb4, 0x00, 0xcc, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // ?

    0x0f, 0xf0, 0x18, 0x18, 0x37, 0xec, 0x2c, 0x74, 0x2b, 0xb4, 0x2b, 0xb4, 0x3c, 0x0c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00,  // @
    0x0f, 0x80, 0x08, 0xf0, 0x0f, 0x1c, 0x01, 0x64, 0x0f, 0x1c, 0x08, 0xf0, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // A
    0x0f, 0xfc, 0x08, 0x04, 0x0b, 0xb4, 0x0b, 0xb4, 0x0c, 0x4c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // B
    0x07, 0xf8, 0x0c, 0x0c, 0x0b, 0xf4, 0x0a, 0x14, 0x0a, 0x14, 0x0e, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // C
    0x0f, 0xfc, 0x08, 0x04, 0x0b, 0xf4, 0x0b, 0xf4, 0x0c, 0x0c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // D
    0x0f, 0xfc, 0x08, 0x04, 0x0b, 0xb4, 0x0a, 0xb4, 0x0a, 0xb4, 0x0e, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // E
    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0xb4, 0x00, 0xb4, 0x00, 0xf4, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // F
    0x07, 0xf8, 0x0c, 0x0c, 0x0b, 0xf4, 0x0b, 0xd4, 0x0b, 0x54, 0x0c, 0x5c, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // G
    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0xbc, 0x00, 0xa0, 0x0f, 0xbc, 0x08, 0x04, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // H
    0x0e, 0x1c, 0x0a, 0x14, 0x0b, 0xf4, 0x08, 0x04, 0x0b, 0xf4, 0x0a, 0x14, 0x0e, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // I
    0x0e, 0x1c, 0x0a, 0x14, 0x0b, 0xf4, 0x0c, 0x04, 0x07, 0xf4, 0x00, 0x14, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // J
    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0xbc, 0x0e, 0x5c, 0x09, 0xe4, 0x0f, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // K
    0x0f, 0xfc, 0x08, 0x04, 0x0b, 0xfc, 0x0a, 0x00, 0x0a, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // L
    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0xec, 0x00, 0xd8, 0x00, 0xb0, 0x00, 0xb0, 0x00, 0xd8, 0x0f, 0xec, 0x08, 0x04, 0x0f, 0xfc,  // M
    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0xcc, 0x07, 0x38, 0x0c, 0xfc, 0x08, 0x04, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // N
    0x07, 0xf8, 0x0c, 0x0c, 0x0b, 0xf4, 0x0a, 0x14, 0x0a, 0x14, 0x0b, 0xf4, 0x0c, 0x0c, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00,  // O

    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0x74, 0x01, 0x74, 0x01, 0x8c, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // P
    0x07, 0xf8, 0x0c, 0x0c, 0x0b, 0xf4, 0x0a, 0x14, 0x0a, 0x14, 0x1b, 0xf4, 0x14, 0x0c, 0x17, 0xf8, 0x1c, 0x00, 0x00, 0x00,  // Q
    0x0f, 0xfc, 0x08, 0x04, 0x0f, 0x74, 0x0e, 0x74, 0x09, 0x8c, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // R
    0x0e, 0xf8, 0x0b, 0x8c, 0x0b, 0x74, 0x0b, 0x54, 0x0c, 0xd4, 0x07, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // S
    0x00, 0x1c, 0x00, 0x14, 0x0f, 0xf4, 0x08, 0x04, 0x0f, 0xf4, 0x00, 0x14, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // T
    0x07, 0xfc, 0x0c, 0x04, 0x0b, 0xfc, 0x0a, 0x00, 0x0b, 0xfc, 0x0c, 0x04, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // U
    0x01, 0xfc, 0x07, 0x04, 0x0c, 0xfc, 0x0b, 0x80, 0x0c, 0xfc, 0x07, 0x04, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // V
    0x01, 0xfc, 0x07, 0x04, 0x0c, 0xfc, 0x0b, 0xc0, 0x0c, 0x40, 0x0b, 0xc0, 0x0c, 0xfc, 0x07, 0x04, 0x01, 0xfc, 0x00, 0x00,  // W
    0x0f, 0x3c, 0x09, 0xe4, 0x0e, 0xdc, 0x03, 0x30, 0x0e, 0xdc, 0x09, 0xe4, 0x0f, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // X
    0x00, 0x3c, 0x00, 0xe4, 0x0f, 0x9c, 0x08, 0x70, 0x0f, 0x9c, 0x00, 0xe4, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Y
    0x0f, 0x1c, 0x09, 0x94, 0x0a, 0xf4, 0x0b, 0x34, 0x0b, 0xd4, 0x0a, 0x64, 0x0e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Z
    0x0f, 0xfc, 0x08, 0x04, 0x0b, 0xf4, 0x0a, 0x14, 0x0e, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 
    0x00, 0x1e, 0x00, 0x72, 0x01, 0xce, 0x07, 0x38, 0x1c, 0xe0, 0x13, 0x80, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // "\"
    0x0