//
// JPEG Decoder
//
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 8/2/2020
// Original JPEG code written 26+ years ago :)
// The goal of this code is to decode baseline JPEG images
// using no more than 18K of RAM (if sent directly to an LCD display)
//
// Copyright 2020 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#include "JPEGDEC.h"

#if defined(ARM_MATH_CM4) || defined(ARM_MATH_CM7)
#define HAS_SIMD
#endif

// forward references
static int JPEGInit(JPEGIMAGE *pJPEG);
static int JPEGParseInfo(JPEGIMAGE *pPage, int bExtractThumb);
static void JPEGGetMoreData(JPEGIMAGE *pPage);
static int DecodeJPEG(JPEGIMAGE *pImage);
static int32_t readRAM(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen);
static int32_t seekMem(JPEGFILE *pFile, int32_t iPosition);
#if defined (__MACH__) || defined( __LINUX__ ) || defined( __MCUXPRESSO )
static int32_t readFile(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen);
static int32_t seekFile(JPEGFILE *pFile, int32_t iPosition);
static void closeFile(void *handle);
#endif
static void JPEGDither(JPEGIMAGE *pJPEG, int iWidth, int iHeight);
/* JPEG tables */
// zigzag ordering of DCT coefficients
static const unsigned char cZigZag[64] = {0,1,5,6,14,15,27,28,
    2,4,7,13,16,26,29,42,
    3,8,12,17,25,30,41,43,
    9,11,18,24,31,40,44,53,
    10,19,23,32,39,45,52,54,
    20,22,33,38,46,51,55,60,
    21,34,37,47,50,56,59,61,
    35,36,48,49,57,58,62,63};

// un-zigzag ordering
static const unsigned char cZigZag2[64] = {0,1,8,16,9,2,3,10,
    17,24,32,25,18,11,4,5,
    12,19,26,33,40,48,41,34,
    27,20,13,6,7,14,21,28,
    35,42,49,56,57,50,43,36,
    29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46,
    53,60,61,54,47,55,62,63};

// For AA&N IDCT method, multipliers are equal to quantization
// coefficients scaled by scalefactor[row]*scalefactor[col], where
// scalefactor[0] = 1
// scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
// For integer operation, the multiplier table is to be scaled by
// IFAST_SCALE_BITS.
static const int iScaleBits[64] = {16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
    21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
    19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
    8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
    4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247};
//
// Range clip and shift for RGB565 output
// input value is 0 to 255, then another 256 for overflow to FF, then 512 more for negative values wrapping around
// Trims a few instructions off the final output stage
//
static const uint8_t ucRangeTable[] = {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
    0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
    0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f};

//
// Convert 8-bit grayscale into RGB565
//
static const uint16_t usGrayTo565[] = {0x0000,0x0000,0x0000,0x0000,0x0020,0x0020,0x0020,0x0020, // 0
    0x0841,0x0841,0x0841,0x0841,0x0861,0x0861,0x0861,0x0861,
    0x1082,0x1082,0x1082,0x1082,0x10a2,0x10a2,0x10a2,0x10a2,
    0x18c3,0x18c3,0x18c3,0x18c3,0x18e3,0x18e3,0x18e3,0x18e3,
    0x2104,0x2104,0x2104,0x2104,0x2124,0x2124,0x2124,0x2124,
    0x2945,0x2945,0x2945,0x2945,0x2965,0x2965,0x2965,0x2965,
    0x3186,0x3186,0x3186,0x3186,0x31a6,0x31a6,0x31a6,0x31a6,
    0x39c7,0x39c7,0x39c7,0x39c7,0x39e7,0x39e7,0x39e7,0x39e7,
    0x4208,0x4208,0x4208,0x4208,0x4228,0x4228,0x4228,0x4228,
    0x4a49,0x4a49,0x4a49,0x4a49,0x4a69,0x4a69,0x4a69,0x4a69,
    0x528a,0x528a,0x528a,0x528a,0x52aa,0x52aa,0x52aa,0x52aa,
    0x5acb,0x5acb,0x5acb,0x5acb,0x5aeb,0x5aeb,0x5aeb,0x5aeb,
    0x630c,0x630c,0x630c,0x630c,0x632c,0x632c,0x632c,0x632c,
    0x6b4d,0x6b4d,0x6b4d,0x6b4d,0x6b6d,0x6b6d,0x6b6d,0x6b6d,
    0x738e,0x738e,0x738e,0x738e,0x73ae,0x73ae,0x73ae,0x73ae,
    0x7bcf,0x7bcf,0x7bcf,0x7bcf,0x7bef,0x7bef,0x7bef,0x7bef,
    0x8410,0x8410,0x8410,0x8410,0x8430,0x8430,0x8430,0x8430,
    0x8c51,0x8c51,0x8c51,0x8c51,0x8c71,0x8c71,0x8c71,0x8c71,
    0x9492,0x9492,0x9492,0x9492,0x94b2,0x94b2,0x94b2,0x94b2,
    0x9cd3,0x9cd3,0x9cd3,0x9cd3,0x9cf3,0x9cf3,0x9cf3,0x9cf3,
    0xa514,0xa514,0xa514,0xa514,0xa534,0xa534,0xa534,0xa534,
    0xad55,0xad55,0xad55,0xad55,0xad75,0xad75,0xad75,0xad75,
    0xb596,0xb596,0xb596,0xb596,0xb5b6,0xb5b6,0xb5b6,0xb5b6,
    0xbdd7,0xbdd7,0xbdd7,0xbdd7,0xbdf7,0xbdf7,0xbdf7,0xbdf7,
    0xc618,0xc618,0xc618,0xc618,0xc638,0xc638,0xc638,0xc638,
    0xce59,0xce59,0xce59,0xce59,0xce79,0xce79,0xce79,0xce79,
    0xd69a,0xd69a,0xd69a,0xd69a,0xd6ba,0xd6ba,0xd6ba,0xd6ba,
    0xdedb,0xdedb,0xdedb,0xdedb,0xdefb,0xdefb,0xdefb,0xdefb,
    0xe71c,0xe71c,0xe71c,0xe71c,0xe73c,0xe73c,0xe73c,0xe73c,
    0xef5d,0xef5d,0xef5d,0xef5d,0xef7d,0xef7d,0xef7d,0xef7d,
    0xf79e,0xf79e,0xf79e,0xf79e,0xf7be,0xf7be,0xf7be,0xf7be,
    0xffdf,0xffdf,0xffdf,0xffdf,0xffff,0xffff,0xffff,0xffff};
//
// Clip and convert red value into 5-bits for RGB565
//
static const uint16_t usRangeTableR[] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // 0
    0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,
    0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
    0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,0x1800,
    0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
    0x2800,0x2800,0x2800,0x2800,0x2800,0x2800,0x2800,0x2800,
    0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,
    0x3800,0x3800,0x3800,0x3800,0x3800,0x3800,0x3800,0x3800,
    0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
    0x4800,0x4800,0x4800,0x4800,0x4800,0x4800,0x4800,0x4800,
    0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,
    0x5800,0x5800,0x5800,0x5800,0x5800,0x5800,0x5800,0x5800,
    0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,
    0x6800,0x6800,0x6800,0x6800,0x6800,0x6800,0x6800,0x6800,
    0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,
    0x7800,0x7800,0x7800,0x7800,0x7800,0x7800,0x7800,0x7800,
    0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,
    0x8800,0x8800,0x8800,0x8800,0x8800,0x8800,0x8800,0x8800,
    0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,
    0x9800,0x9800,0x9800,0x9800,0x9800,0x9800,0x9800,0x9800,
    0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,
    0xa800,0xa800,0xa800,0xa800,0xa800,0xa800,0xa800,0xa800,
    0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,
    0xb800,0xb800,0xb800,0xb800,0xb800,0xb800,0xb800,0xb800,
    0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,
    0xc800,0xc800,0xc800,0xc800,0xc800,0xc800,0xc800,0xc800,
    0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,
    0xd800,0xd800,0xd800,0xd800,0xd800,0xd800,0xd800,0xd800,
    0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,
    0xe800,0xe800,0xe800,0xe800,0xe800,0xe800,0xe800,0xe800,
    0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800, // 256
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 512
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 768
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//
// Clip and convert green value into 5-bits for RGB565
//
static const uint16_t usRangeTableG[] = {0x0000,0x0000,0x0000,0x0000,0x0020,0x0020,0x0020,0x0020, // 0
    0x0040,0x0040,0x0040,0x0040,0x0060,0x0060,0x0060,0x0060,
    0x0080,0x0080,0x0080,0x0080,0x00a0,0x00a0,0x00a0,0x00a0,
    0x00c0,0x00c0,0x00c0,0x00c0,0x00e0,0x00e0,0x00e0,0x00e0,
    0x0100,0x0100,0x0100,0x0100,0x0120,0x0120,0x0120,0x0120,
    0x0140,0x0140,0x0140,0x0140,0x0160,0x0160,0x0160,0x0160,
    0x0180,0x0180,0x0180,0x0180,0x01a0,0x01a0,0x01a0,0x01a0,
    0x01c0,0x01c0,0x01c0,0x01c0,0x01e0,0x01e0,0x01e0,0x01e0,
    0x0200,0x0200,0x0200,0x0200,0x0220,0x0220,0x0220,0x0220,
    0x0240,0x0240,0x0240,0x0240,0x0260,0x0260,0x0260,0x0260,
    0x0280,0x0280,0x0280,0x0280,0x02a0,0x02a0,0x02a0,0x02a0,
    0x02c0,0x02c0,0x02c0,0x02c0,0x02e0,0x02e0,0x02e0,0x02e0,
    0x0300,0x0300,0x0300,0x0300,0x0320,0x0320,0x0320,0x0320,
    0x0340,0x0340,0x0340,0x0340,0x0360,0x0360,0x0360,0x0360,
    0x0380,0x0380,0x0380,0x0380,0x03a0,0x03a0,0x03a0,0x03a0,
    0x03c0,0x03c0,0x03c0,0x03c0,0x03e0,0x03e0,0x03e0,0x03e0,
    0x0400,0x0400,0x0400,0x0400,0x0420,0x0420,0x0420,0x0420,
    0x0440,0x0440,0x0440,0x0440,0x0460,0x0460,0x0460,0x0460,
    0x0480,0x0480,0x0480,0x0480,0x04a0,0x04a0,0x04a0,0x04a0,
    0x04c0,0x04c0,0x04c0,0x04c0,0x04e0,0x04e0,0x04e0,0x04e0,
    0x0500,0x0500,0x0500,0x0500,0x0520,0x0520,0x0520,0x0520,
    0x0540,0x0540,0x0540,0x0540,0x0560,0x0560,0x0560,0x0560,
    0x0580,0x0580,0x0580,0x0580,0x05a0,0x05a0,0x05a0,0x05a0,
    0x05c0,0x05c0,0x05c0,0x05c0,0x05e0,0x05e0,0x05e0,0x05e0,
    0x0600,0x0600,0x0600,0x0600,0x0620,0x0620,0x0620,0x0620,
    0x0640,0x0640,0x0640,0x0640,0x0660,0x0660,0x0660,0x0660,
    0x0680,0x0680,0x0680,0x0680,0x06a0,0x06a0,0x06a0,0x06a0,
    0x06c0,0x06c0,0x06c0,0x06c0,0x06e0,0x06e0,0x06e0,0x06e0,
    0x0700,0x0700,0x0700,0x0700,0x0720,0x0720,0x0720,0x0720,
    0x0740,0x0740,0x0740,0x0740,0x0760,0x0760,0x0760,0x0760,
    0x0780,0x0780,0x0780,0x0780,0x07a0,0x07a0,0x07a0,0x07a0,
    0x07c0,0x07c0,0x07c0,0x07c0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0, // 256
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 512
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 768
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//
// Clip and convert blue value into 5-bits for RGB565
//
static const uint16_t usRangeTableB[] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // 0
    0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,
    0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,
    0x0003,0x0003,0x0003,0x0003,0x0003,0x0003,0x0003,0x0003,
    0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,
    0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,
    0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,
    0x0007,0x0007,0x0007,0x0007,0x0007,0x0007,0x0007,0x0007,
    0x0008,0x0008,0x0008,0x0008,0x0008,0x0008,0x0008,0x0008,
    0x0009,0x0009,0x0009,0x0009,0x0009,0x0009,0x0009,0x0009,
    0x000a,0x000a,0x000a,0x000a,0x000a,0x000a,0x000a,0x000a,
    0x000b,0x000b,0x000b,0x000b,0x000b,0x000b,0x000b,0x000b,
    0x000c,0x000c,0x000c,0x000c,0x000c,0x000c,0x000c,0x000c,
    0x000d,0x000d,0x000d,0x000d,0x000d,0x000d,0x000d,0x000d,
    0x000e,0x000e,0x000e,0x000e,0x000e,0x000e,0x000e,0x000e,
    0x000f,0x000f,0x000f,0x000f,0x000f,0x000f,0x000f,0x000f,
    0x0010,0x0010,0x0010,0x0010,0x0010,0x0010,0x0010,0x0010,
    0x0011,0x0011,0x0011,0x0011,0x0011,0x0011,0x0011,0x0011,
    0x0012,0x0012,0x0012,0x0012,0x0012,0x0012,0x0012,0x0012,
    0x0013,0x0013,0x0013,0x0013,0x0013,0x0013,0x0013,0x0013,
    0x0014,0x0014,0x0014,0x0014,0x0014,0x0014,0x0014,0x0014,
    0x0015,0x0015,0x0015,0x0015,0x0015,0x0015,0x0015,0x0015,
    0x0016,0x0016,0x0016,0x0016,0x0016,0x0016,0x0016,0x0016,
    0x0017,0x0017,0x0017,0x0017,0x0017,0x0017,0x0017,0x0017,
    0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,
    0x0019,0x0019,0x0019,0x0019,0x0019,0x0019,0x0019,0x0019,
    0x001a,0x001a,0x001a,0x001a,0x001a,0x001a,0x001a,0x001a,
    0x001b,0x001b,0x001b,0x001b,0x001b,0x001b,0x001b,0x001b,
    0x001c,0x001c,0x001c,0x001c,0x001c,0x001c,0x001c,0x001c,
    0x001d,0x001d,0x001d,0x001d,0x001d,0x001d,0x001d,0x001d,
    0x001e,0x001e,0x001e,0x001e,0x001e,0x001e,0x001e,0x001e,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f, // 256
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 512
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 768
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#if defined (__MACH__) || defined( __LINUX__ ) || defined( __MCUXPRESSO )
//
// API for C
//

//
// Memory initialization
//
int JPEG_openRAM(JPEGIMAGE *pJPEG, uint8_t *pData, int iDataSize, JPEG_DRAW_CALLBACK *pfnDraw)
{
    memset(pJPEG, 0, sizeof(JPEGIMAGE));
    pJPEG->ucMemType = JPEG_MEM_RAM;
    pJPEG->pfnRead = readRAM;
    pJPEG->pfnSeek = seekMem;
    pJPEG->pfnDraw = pfnDraw;
    pJPEG->pfnOpen = NULL;
    pJPEG->pfnClose = NULL;
    pJPEG->JPEGFile.iSize = iDataSize;
    pJPEG->JPEGFile.pData = pData;
    pJPEG->iMaxMCUs = 1000; // set to an unnaturally high value to start
    return JPEGInit(pJPEG);
} /* JPEG_openRAM() */
//
// File initialization
//
int JPEG_openFile(JPEGIMAGE *pJPEG, const char *szFilename, JPEG_DRAW_CALLBACK *pfnDraw)
{
    memset(pJPEG, 0, sizeof(JPEGIMAGE));
    pJPEG->ucMemType = JPEG_MEM_RAM;
    pJPEG->pfnRead = readFile;
    pJPEG->pfnSeek = seekFile;
    pJPEG->pfnDraw = pfnDraw;
    pJPEG->pfnOpen = NULL;
    pJPEG->pfnClose = closeFile;
    pJPEG->iMaxMCUs = 1000; // set to an unnaturally high value to start
    pJPEG->JPEGFile.fHandle = fopen(szFilename, "r+b");
    if (pJPEG->JPEGFile.fHandle == NULL)
       return 0;
    fseek((FILE *)pJPEG->JPEGFile.fHandle, 0, SEEK_END);
    pJPEG->JPEGFile.iSize = (int)ftell((FILE *)pJPEG->JPEGFile.fHandle);
    fseek((FILE *)pJPEG->JPEGFile.fHandle, 0, SEEK_SET);
    return JPEGInit(pJPEG);
} /* JPEG_openFile() */

int JPEG_getLastError(JPEGIMAGE *pJPEG)
{
    return pJPEG->iError;
} /* JPEG_getLastError() */

int JPEG_getWidth(JPEGIMAGE *pJPEG)
{
    return pJPEG->iWidth;
} /* JPEG_getWidth() */

int JPEG_getHeight(JPEGIMAGE *pJPEG)
{
    return pJPEG->iHeight;
} /* JPEG_getHeight() */

int JPEG_getOrientation(JPEGIMAGE *pJPEG)
{
    return (int)pJPEG->ucOrientation;
} /* JPEG_getOrientation() */

int JPEG_getBpp(JPEGIMAGE *pJPEG)
{
    return (int)pJPEG->ucBpp;
} /* JPEG_getBpp() */
int JPEG_getSubSample(JPEGIMAGE *pJPEG)
{
    return (int)pJPEG->ucSubSample;
} /* JPEG_getSubSample() */
int JPEG_hasThumb(JPEGIMAGE *pJPEG)
{
    return (int)pJPEG->ucHasThumb;
} /* JPEG_hasThumb() */
int JPEG_getThumbWidth(JPEGIMAGE *pJPEG)
{
    return pJPEG->iThumbWidth;
} /* JPEG_getThumbWidth() */
int JPEG_getThumbHeight(JPEGIMAGE *pJPEG)
{
    return pJPEG->iThumbHeight;
} /* JPEG_getThumbHeight() */
void JPEG_setPixelType(JPEGIMAGE *pJPEG, int iType)
{
    pJPEG->ucPixelType = (uint8_t)iType;
} /* JPEG_setPixelType() */
void JPEG_setMaxOutputSize(JPEGIMAGE *pJPEG, int iMaxMCUs)
{
    if (iMaxMCUs < 1)
        iMaxMCUs = 1; // don't allow invalid value
    pJPEG->iMaxMCUs = iMaxMCUs;
} /* JPEG_setMaxOutputSize() */

int JPEG_decode(JPEGIMAGE *pJPEG, int x, int y, int iOptions)
{
    pJPEG->iXOffset = x;
    pJPEG->iYOffset = y;
    pJPEG->iOptions = iOptions;
    return DecodeJPEG(pJPEG);
} /* JPEG_decode() */

int JPEG_decodeDither(JPEGIMAGE *pJPEG, uint8_t *pDither, int iOptions)
{
    pJPEG->iOptions = iOptions;
    pJPEG->pDitherBuffer = pDither;
    return DecodeJPEG(pJPEG);
} /* JPEG_decodeDither() */

void JPEG_close(JPEGIMAGE *pJPEG)
{
    if (pJPEG->pfnClose)
        (*pJPEG->pfnClose)(pJPEG->JPEGFile.fHandle);
} /* JPEG_close() */

#endif // !__cplusplus
//
// Helper functions for memory based images
//
static int32_t readRAM(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    memcpy(pBuf, &pFile->pData[pFile->iPos], iBytesRead);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readRAM() */

static int32_t readFLASH(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    memcpy_P(pBuf, &pFile->pData[pFile->iPos], iBytesRead);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readFLASH() */

static int32_t seekMem(JPEGFILE *pFile, int32_t iPosition)
{
    if (iPosition < 0) iPosition = 0;
    else if (iPosition >= pFile->iSize) iPosition = pFile->iSize-1;
    pFile->iPos = iPosition;
    return iPosition;
} /* seekMem() */

#if defined (__MACH__) || defined( __LINUX__ ) || defined( __MCUXPRESSO )

static void closeFile(void *handle)
{
    fclose((FILE *)handle);
} /* closeFile() */

static int32_t seekFile(JPEGFILE *pFile, int32_t iPosition)
{
    if (iPosition < 0) iPosition = 0;
    else if (iPosition >= pFile->iSize) iPosition = pFile->iSize-1;
    pFile->iPos = iPosition;
    fseek((FILE *)pFile->fHandle, iPosition, SEEK_SET);
    return iPosition;
} /* seekFile() */

static int32_t readFile(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int)fread(pBuf, 1, iBytesRead, (FILE *)pFile->fHandle);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readFile() */

#endif // __LINUX__
//
// The following functions are written in plain C and have no
// 3rd party dependencies, not even the C runtime library
//
//
// Initialize a JPEG file and callback access from a file on SD or memory
// returns 1 for success, 0 for failure
// Fills in the basic image info fields of the JPEGIMAGE structure
//
static int JPEGInit(JPEGIMAGE *pJPEG)
{
    return JPEGParseInfo(pJPEG, 0); // gather info for image
} /* JPEGInit() */
//
// Unpack the Huffman tables
//
static int JPEGGetHuffTables(uint8_t *pBuf, int iLen, JPEGIMAGE *pJPEG)
{
    int i, j, iOffset, iTableOffset;
    uint8_t ucTable, *pHuffVals;
    
    iOffset = 0;
    pHuffVals = (uint8_t *)pJPEG->usPixels; // temp holding area to save RAM
    while (iLen > 17)  // while there are tables to copy (we may have combined more than 1 table together)
    {
        ucTable = pBuf[iOffset++]; // get table index
        if (ucTable & 0x10) // convert AC offset of 0x10 into offset of 4
            ucTable ^= 0x14;
        pJPEG->ucHuffTableUsed |= (1 << ucTable); // mark this table as being defined
        if (ucTable <= 7) // tables are 0-3, AC+DC
        {
            iTableOffset = ucTable * HUFF_TABLEN;
            j = 0; // total bits
            for (i=0; i<16; i++)
            {
                j += pBuf[iOffset];
                pHuffVals[iTableOffset+i] = pBuf[iOffset++];
            }
            iLen -= 17; // subtract length of bit lengths
            if (j == 0 || j > 256 || j > iLen) // bogus bit lengths
            {
                return -1;
            }
            iTableOffset += 16;
            for (i=0; i<j; i++)
            {  // copy huffman table
                pHuffVals[iTableOffset+i] = pBuf[iOffset++];
            }
            iLen -= j;
        }
    }
    return 0;
} /* JPEGGetHuffTables() */
#ifdef FUTURE
//
// Create 11-bit lookup tables for some images where it doesn't work
// for 10-bit tables
//
static int JPEGMakeHuffTables_Slow(JPEGIMAGE *pJPEG, int bThumbnail)
{
    int code, repeat, count, codestart;
    int j;
    int iLen, iTable;
    unsigned short *pTable, *pShort, *pLong;
    unsigned char *pucTable, *pucShort, *pucLong;
    uint32_t ul, *pLongTable;
    int iBitNum; // current code bit length
    int cc; // code
    unsigned char *p, *pBits, ucCode;
    int iMaxLength, iMaxMask;

    pJPEG->b11Bit = 1; // indicate we're using the bigger A/C decode tables
    // first do DC components (up to 4 tables of 12-bit codes)
    // we can save time and memory for the DC codes by knowing that there exist short codes (<= 6 bits)
    // and long codes (>6 bits, but the first 5 bits are 1's).  This allows us to create 2 tables: a 6-bit and 7 or 8-bit
    // to handle any DC codes
    iMaxLength = 12; // assume DC codes can be 12-bits
    iMaxMask = 0x7f; // lower 7 bits after truncate 5 leading 1's
    if (pJPEG->ucMode == 0xc3) // create 13-bit tables for lossless mode
    {
        iMaxLength = 13;
        iMaxMask = 0xff;
    }
    for (iTable = 0; iTable < 2; iTable++)
    {
        if (pJPEG->ucHuffTableUsed & (1<<iTable))
        {
            //         pJPEG->huffdcFast[iTable] = (int *)PILIOAlloc(0x180); // short table = 128 bytes, long table = 256 bytes
            pucShort = (unsigned char *)&pJPEG->ucHuffDC[iTable*DC_TABLE_SIZE];
            //         pJPEG->huffdc[iTable] = pJPEG->huffdcFast[iTable] + 0x20; // 0x20 longs = 128 bytes
            pucLong = (unsigned char *)&pJPEG->ucHuffDC[iTable*DC_TABLE_SIZE + 128];
            pBits = &pJPEG->ucHuffVals[iTable * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            cc = 0; // start with a code of 0
            for (iBitNum = 1; iBitNum <= 16; iBitNum++)
            {
                iLen = *pBits++; // get number of codes for this bit length
                if (iBitNum > iMaxLength && iLen > 0) // we can't handle codes longer a certain length
                {
                    return -1;
                }
                while (iLen)
                {
                    //               if (iBitNum > 6) // do long table
                    if ((cc >> (iBitNum-5)) == 0x1f) // first 5 bits are 1 - use long table
                    {
                        count = iMaxLength - iBitNum;
                        codestart = cc << count;
                        pucTable = &pucLong[codestart & iMaxMask]; // use lower 7/8 bits of code
                    }
                    else // do short table
                    {
                        count = 6 - iBitNum;
                        if (count < 0)
                            return -1; // DEBUG - something went wrong
                        codestart = cc << count;
                        pucTable = &pucShort[codestart];
                    }
                    ucCode = *p++;  // get actual huffman code
                    if (ucCode == 16 && pJPEG->ucMode == 0xc3) // lossless mode
                    {
                        // in lossless mode, this code won't fit in 4 bits, so save it's length in the next slot
                        ucCode = 255;
                        pucLong[256] = (unsigned char)iBitNum;
                    }
                    // does precalculating the DC value save time on ARM?
#ifndef USE_ARM_ASM
                    if (ucCode != 0 && (ucCode + iBitNum) <= 6 && pJPEG->ucMode != 0xc2) // we can fit the magnitude value in the code lookup (not for progressive)
                    {
                        int k, iLoop;
                        unsigned char ucCoeff;
                        unsigned char *d = &pucTable[512];
                        unsigned char ucMag = ucCode;
                        ucCode |= ((iBitNum+ucCode) << 4); // add magnitude bits to length
                        repeat = 1<<ucMag;
                        iLoop = 1<<(count-ucMag);
                        for (j=0; j<repeat; j++)
                        { // calcuate the magnitude coeff already
                            if (j & 1<<(ucMag-1)) // positive number
                                ucCoeff = (unsigned char)j;
                            else // negative number
                                ucCoeff = (unsigned char)(j - ((1<<ucMag)-1));
                            for (k=0; k<iLoop; k++)
                            {
                                *d++ = ucCoeff;
                            } // for k
                        } // for j
                    }
#endif
                    else
                    {
                        ucCode |= (iBitNum << 4);
                    }
                    if (count) // do it as dwords to save time
                    {
                        repeat = (1<<count);
                        memset(pucTable, ucCode, repeat);
                        //                  pLongTable = (uint32_t *)pTable;
                        //                  repeat = 1 << (count-2); // store as dwords (/4)
                        //                  ul = code | (code << 16);
                        //                  for (j=0; j<repeat; j++)
                        //                     *pLongTable++ = ul;
                    }
                    else
                    {
                        pucTable[0] = ucCode;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            }
        } // if table defined
    }
    // now do AC components (up to 2 tables of 16-bit codes)
    // We split the codes into a short table (9 bits or less) and a long table (first 5 bits are 1)
    for (iTable = 0; iTable < 2; iTable++)
    {
        if (pJPEG->ucHuffTableUsed & (1<<(iTable+4)))  // if this table is defined
        {
            pBits = &pJPEG->ucHuffVals[(iTable+4) * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            pShort = &pJPEG->usHuffAC[iTable*HUFF11SIZE];
            pLong = &pJPEG->usHuffAC[iTable*HUFF11SIZE + 1024]; // long codes start here
            cc = 0; // start with a code of 0
            // construct the decode table
            for (iBitNum = 1; iBitNum <= 16; iBitNum++)
            {
                iLen = *pBits++; // get number of codes for this bit length
                while (iLen)
                {
                    if ((cc >> (iBitNum-4)) == 0xf) // first 4 bits are 1 - use long table
                    {
                        count = 16 - iBitNum;
                        codestart = cc << count;
                        pTable = &pLong[codestart & 0xfff]; // use lower 12 bits of code
                    }
                    else
                    {
                        count = 12 - iBitNum;
                        if (count < 0) // a 13-bit? code - that doesn't fit our optimized scheme, see if we can do a bigger table version
                        {
                            return -1; // DEBUG - fatal error, we currently don't support it
                        }
                        codestart = cc << count;
                        pTable = &pShort[codestart]; // 11 bits or shorter
                    }
                    code = *p++;  // get actual huffman code
                    if (bThumbnail && code != 0) // add "extra" bits to code length since we skip these codes
                    {
                        // get rid of extra bits in code and add increment (1) for AC index
                        code = ((iBitNum+(code & 0xf)) << 8) | ((code >> 4)+1);
                    }
                    else
                    {
                        code |= (iBitNum << 8);
                    }
                    if (count) // do it as dwords to save time
                    {
                        repeat = 1 << (count-1); // store as dwords (/2)
                        ul = code | (code << 16);
                        pLongTable = (uint32_t *)pTable;
                        for (j=0; j<repeat; j++)
                            *pLongTable++ = ul;
                    }
                    else
                    {
                        pTable[0] = (unsigned short)code;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            } // for each bit length
        } // if table defined
    }
    return 0;
} /* JPEGMakeHuffTables_Slow() */
#endif // FUTURE
//
// Expand the Huffman tables for fast decoding
// returns 1 for success, 0 for failure
//
static int JPEGMakeHuffTables(JPEGIMAGE *pJPEG, int bThumbnail)
{
    int code, repeat, count, codestart;
    int j;
    int iLen, iTable;
    uint16_t *pTable, *pShort, *pLong;
    uint8_t *pHuffVals, *pucTable, *pucShort, *pucLong;
    uint32_t ul, *pLongTable;
    int iBitNum; // current code bit length
    int cc; // code
    uint8_t *p, *pBits, ucCode;
    int iMaxLength, iMaxMask;
    int iTablesUsed;
    
    iTablesUsed = 0;
    pHuffVals = (uint8_t *)pJPEG->usPixels;
    for (j=0; j<4; j++)
    {
        if (pJPEG->ucHuffTableUsed & (1 << j))
            iTablesUsed++;
    }
    // first do DC components (up to 4 tables of 12-bit codes)
    // we can save time and memory for the DC codes by knowing that there exist short codes (<= 6 bits)
    // and long codes (>6 bits, but the first 5 bits are 1's).  This allows us to create 2 tables: a 6-bit and 7 or 8-bit
    // to handle any DC codes
    iMaxLength = 12; // assume DC codes can be 12-bits
    iMaxMask = 0x7f; // lower 7 bits after truncate 5 leading 1's
    for (iTable = 0; iTable < 4; iTable++)
    {
        if (pJPEG->ucHuffTableUsed & (1 << iTable))
        {
            //         pJPEG->huffdcFast[iTable] = (int *)PILIOAlloc(0x180); // short table = 128 bytes, long table = 256 bytes
            pucShort = &pJPEG->ucHuffDC[iTable*DC_TABLE_SIZE];
            //         pJPEG->huffdc[iTable] = pJPEG->huffdcFast[iTable] + 0x20; // 0x20 longs = 128 bytes
            pucLong = &pJPEG->ucHuffDC[iTable*DC_TABLE_SIZE + 128];
            pBits = &pHuffVals[iTable * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            cc = 0; // start with a code of 0
            for (iBitNum = 1; iBitNum <= 16; iBitNum++)
            {
                iLen = *pBits++; // get number of codes for this bit length
                if (iBitNum > iMaxLength && iLen > 0) // we can't handle codes longer a certain length
                {
                    return 0;
                }
                while (iLen)
                {
                    //               if (iBitNum > 6) // do long table
                    if ((cc >> (iBitNum-5)) == 0x1f) // first 5 bits are 1 - use long table
                    {
                        count = iMaxLength - iBitNum;
                        codestart = cc << count;
                        pucTable = &pucLong[codestart & iMaxMask]; // use lower 7/8 bits of code
                    }
                    else // do short table
                    {
                        count = 6 - iBitNum;
                        if (count < 0)
                            return 0; // DEBUG - something went wrong
                        codestart = cc << count;
                        pucTable = &pucShort[codestart];
                    }
                    ucCode = *p++;  // get actual huffman code
                    // does precalculating the DC value save time on ARM?
#ifndef USE_ARM_ASM
                    if (ucCode != 0 && (ucCode + iBitNum) <= 6 && pJPEG->ucMode != 0xc2) // we can fit the magnitude value in the code lookup (not for progressive)
                    {
                        int k, iLoop;
                        unsigned char ucCoeff;
                        unsigned char *d = &pucTable[512];
                        unsigned char ucMag = ucCode;
                        ucCode |= ((iBitNum+ucCode) << 4); // add magnitude bits to length
                        repeat = 1<<ucMag;
                        iLoop = 1<<(count-ucMag);
                        for (j=0; j<repeat; j++)
                        { // calcuate the magnitude coeff already
                            if (j & 1<<(ucMag-1)) // positive number
                                ucCoeff = (unsigned char)j;
                            else // negative number
                                ucCoeff = (unsigned char)(j - ((1<<ucMag)-1));
                            for (k=0; k<iLoop; k++)
                            {
                                *d++ = ucCoeff;
                            } // for k
                        } // for j
                    }
#endif
                    else
                    {
                        ucCode |= (iBitNum << 4);
                    }
                    if (count) // do it as dwords to save time
                    {
                        repeat = (1<<count);
                        memset(pucTable, ucCode, repeat);
                        //                  pLongTable = (uint32_t *)pTable;
                        //                  repeat = 1 << (count-2); // store as dwords (/4)
                        //                  ul = code | (code << 16);
                        //                  for (j=0; j<repeat; j++)
                        //                     *pLongTable++ = ul;
                    }
                    else
                    {
                        pucTable[0] = ucCode;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            }
        } // if table defined
    }
    // now do AC components (up to 4 tables of 16-bit codes)
    // We split the codes into a short table (9 bits or less) and a long table (first 5 bits are 1)
    for (iTable = 0; iTable < 4; iTable++)
    {
        if (pJPEG->ucHuffTableUsed & (1 << (iTable+4)))  // if this table is defined
        {
            pBits = &pHuffVals[(iTable+4) * HUFF_TABLEN];
            p = pBits;
            p += 16; // point to bit data
            pShort = &pJPEG->usHuffAC[iTable*HUFF11SIZE];
            pLong = &pJPEG->usHuffAC[iTable*HUFF11SIZE + 1024];
            cc = 0; // start with a code of 0
            // construct the decode table
            for (iBitNum = 1; iBitNum <= 16; iBitNum++)
            {
                iLen = *pBits++; // get number of codes for this bit length
                while (iLen)
                {
                    if ((cc >> (iBitNum-6)) == 0x3f) // first 6 bits are 1 - use long table
                    {
                        count = 16 - iBitNum;
                        codestart = cc << count;
                        pTable = &pLong[codestart & 0x3ff]; // use lower 10 bits of code
                    }
                    else
                    {
                        count = 10 - iBitNum;
                        if (count < 0) // an 11/12-bit? code - that doesn't fit our optimized scheme, see if we can do a bigger table version
                        {
                            if (count == -1 && iTablesUsed <= 4) // we need to create "slow" tables
                            { // DEBUG
//                                j = JPEGMakeHuffTables_Slow(pJPEG, bThumbnail);
                                return 0;
                            }
                            else
                                return 0; // DEBUG - fatal error, more than 2 big tables we currently don't support
                        }
                        codestart = cc << count;
                        pTable = &pShort[codestart]; // 10 bits or shorter
                    }
                    code = *p++;  // get actual huffman code
                    if (bThumbnail && code != 0) // add "extra" bits to code length since we skip these codes
                    {
                        // get rid of extra bits in code and add increment (1) for AC index
                        code = ((iBitNum+(code & 0xf)) << 8) | ((code >> 4)+1);
                    }
#ifdef BOGUS // precalculating the AC coeff makes it run slightly slower
                    else if ((code & 0xf) != 0 && (code + iBitNum) <= 10) // we can fit the magnitude value + huffman code in a single read
                    {
                        int k, iLoop;
                        unsigned short usCoeff;
                        unsigned short *d = &pTable[4096]; // use unused table slots 2+3 for extra coeff data
                        unsigned char ucMag = (unsigned char)(code & 0xf);
                        code |= ((iBitNum + (code & 0xf)) << 8); // add magnitude bits to length
                        repeat = 1<<ucMag;
                        iLoop = 1<<(count-ucMag);
                        for (j=0; j<repeat; j++)
                        { // calcuate the magnitude coeff already
                            if (j & 1<<(ucMag-1)) // positive number
                                usCoeff = (unsigned short)j;
                            else // negative number
                                usCoeff = (unsigned short)(j - ((1<<ucMag)-1));
                            for (k=0; k<iLoop; k++)
                            {
                                *d++ = usCoeff;
                            } // for k
                        } // for j
                    }
#endif
                    else
                    {
                        code |= (iBitNum << 8);
                    }
                    if (count) // do it as dwords to save time
                    {
                        repeat = 1 << (count-1); // store as dwords (/2)
                        ul = code | (code << 16);
                        pLongTable = (uint32_t *)pTable;
                        for (j=0; j<repeat; j++)
                            *pLongTable++ = ul;
                    }
                    else
                    {
                        pTable[0] = (unsigned short)code;
                    }
                    cc++;
                    iLen--;
                }
                cc <<= 1;
            } // for each bit length
        } // if table defined
    }
    return 1;
} /* JPEGMakeHuffTables() */

//
// TIFFSHORT
// read a 16-bit unsigned integer from the given pointer
// and interpret the data as big endian (Motorola) or little endian (Intel)
//
static uint16_t TIFFSHORT(unsigned char *p, int bMotorola)
{
    unsigned short s;

    if (bMotorola)
        s = *p * 0x100 + *(p+1); // big endian (AKA Motorola byte order)
    else
        s = *p + *(p+1)*0x100; // little endian (AKA Intel byte order)
    return s;
} /* TIFFSHORT() */
//
// TIFFLONG
// read a 32-bit unsigned integer from the given pointer
// and interpret the data as big endian (Motorola) or little endian (Intel)
//
static uint32_t TIFFLONG(unsigned char *p, int bMotorola)
{
    uint32_t l;

    if (bMotorola)
        l = *p * 0x1000000 + *(p+1) * 0x10000 + *(p+2) * 0x100 + *(p+3); // big endian
    else
        l = *p + *(p+1) * 0x100 + *(p+2) * 0x10000 + *(p+3) * 0x1000000; // little endian
    return l;
} /* TIFFLONG() */
//
// TIFFVALUE
// read an integer value encoded in a TIFF TAG (12-byte structure)
// and interpret the data as big endian (Motorola) or little endian (Intel)
//
static int TIFFVALUE(unsigned char *p, int bMotorola)
{
    int i, iType;
    
    iType = TIFFSHORT(p+2, bMotorola);
    /* If pointer to a list of items, must be a long */
    if (TIFFSHORT(p+4, bMotorola) > 1)
    {
        iType = 4;
    }
    switch (iType)
    {
        case 3: /* Short */
            i = TIFFSHORT(p+8, bMotorola);
            break;
        case 4: /* Long */
        case 7: // undefined (treat it as a long since it's usually a multibyte buffer)
            i = TIFFLONG(p+8, bMotorola);
            break;
        case 6: // signed byte
            i = (signed char)p[8];
            break;
        case 2: /* ASCII */
        case 5: /* Unsigned Rational */
        case 10: /* Signed Rational */
            i = TIFFLONG(p+8, bMotorola);
            break;
        default: /* to suppress compiler warning */
            i = 0;
            break;
    }
    return i;
    
} /* TIFFVALUE() */
static void GetTIFFInfo(JPEGIMAGE *pPage, int bMotorola, int iOffset)
{
    int iTag, iTagCount, i;
    uint8_t *cBuf = pPage->ucFileBuf;
    
    iTagCount = TIFFSHORT(&cBuf[iOffset], bMotorola);  /* Number of tags in this dir */
    if (iTagCount < 1 || iTagCount > 256) // invalid tag count
        return; /* Bad header info */
    /*--- Search the TIFF tags ---*/
    for (i=0; i<iTagCount; i++)
    {
        unsigned char *p = &cBuf[iOffset + (i*12) +2];
        iTag = TIFFSHORT(p, bMotorola);  /* current tag value */
        if (iTag == 274) // orientation tag
        {
            pPage->ucOrientation = TIFFVALUE(p, bMotorola);
        }
        else if (iTag == 256) // width of thumbnail
        {
            pPage->iThumbWidth = TIFFVALUE(p, bMotorola);
        }
        else if (iTag == 257) // height of thumbnail
        {
            pPage->iThumbHeight = TIFFVALUE(p, bMotorola);
        }
        else if (iTag == 513) // offset to JPEG data
        {
            pPage->iThumbData = TIFFVALUE(p, bMotorola);
        }
    }
} /* GetTIFFInfo() */

static int JPEGGetSOS(JPEGIMAGE *pJPEG, int *iOff)
{
    int16_t sLen;
    int iOffset = *iOff;
    int i, j;
    uint8_t uc,c,cc;
    uint8_t *buf = pJPEG->ucFileBuf;
    
    sLen = MOTOSHORT(&buf[iOffset]);
    iOffset += 2;
    
    // Assume no components in this scan
    for (i=0; i<4; i++)
        pJPEG->JPCI[i].component_needed = 0;
    
    uc = buf[iOffset++]; // get number of components
    pJPEG->ucComponentsInScan = uc;
    sLen -= 3;
    if (uc < 1 || uc > MAX_COMPS_IN_SCAN || sLen != (uc*2+3)) // check length of data packet
        return 1; // error
    for (i=0; i<uc; i++)
    {
        cc = buf[iOffset++];
        c = buf[iOffset++];
        sLen -= 2;
        for (j=0; j<4; j++) // search for component id
        {
            if (pJPEG->JPCI[j].component_id == cc)
                break;
        }
        if (j == 4) // error, not found
            return 1;
        if ((c & 0xf) > 3 || (c & 0xf0) > 0x30)
            return 1; // bogus table numbers
        pJPEG->JPCI[j].dc_tbl_no = c >> 4;
        pJPEG->JPCI[j].ac_tbl_no = c & 0xf;
        pJPEG->JPCI[j].component_needed = 1; // mark this component as being included in the scan
    }
    pJPEG->iScanStart = buf[iOffset++]; // Get the scan start (or lossless predictor) for this scan
    pJPEG->iScanEnd = buf[iOffset++]; // Get the scan end for this scan
    c = buf[iOffset++]; // successive approximation bits
    pJPEG->cApproxBitsLow = c & 0xf; // also point transform in lossless mode
    pJPEG->cApproxBitsHigh = c >> 4;
    
    *iOff = iOffset;
    return 0;
    
} /* JPEGGetSOS() */
//
// Remove markers from the data stream to allow faster decode
// Stuffed zeros and restart interval markers aren't needed to properly decode
// the data, but they make reading VLC data slower, so I pull them out first
//
static int JPEGFilter(uint8_t *pBuf, uint8_t *d, int iLen, uint8_t *bFF)
{
    // since we have the entire jpeg buffer in memory already, we can just change it in place
    unsigned char c, *s, *pEnd, *pStart;
    
    pStart = d;
    s = pBuf;
    pEnd = &s[iLen-1]; // stop just shy of the end to not miss a final marker/stuffed 0
    if (*bFF) // last byte was a FF, check the next one
    {
        if (s[0] == 0) // stuffed 0, keep the FF
            *d++ = 0xff;
        s++;
        *bFF = 0;
    }
    while (s < pEnd)
    {
        c = *d++ = *s++;
        if (c == 0xff) // marker or stuffed zeros?
        {
            if (s[0] != 0) // it's a marker, skip both
            {
                d--;
            }
            s++; // for stuffed 0's, store the FF, skip the 00
        }
    }
    if (s == pEnd) // need to test the last byte
    {
        c = s[0];
        if (c == 0xff) // last byte is FF, take care of it next time through
            *bFF = 1; // take care of it next time through
        else
            *d++ = c; // nope, just store it
    }
    return (int)(d-pStart); // filtered output length
} /* JPEGFilter() */
//
// Read and filter more VLC data for decoding
//
static void JPEGGetMoreData(JPEGIMAGE *pPage)
{
    int iDelta = pPage->iVLCSize - pPage->iVLCOff;
//    printf("Getting more data...size=%d, off=%d\n", pPage->iVLCSize, pPage->iVLCOff);
    // move any existing data down
    if (iDelta >= (JPEG_FILE_BUF_SIZE-64) || iDelta < 0)
        return; // buffer is already full; no need to read more data
    if (pPage->iVLCOff != 0)
    {
        memcpy(pPage->ucFileBuf, &pPage->ucFileBuf[pPage->iVLCOff], pPage->iVLCSize - pPage->iVLCOff);
        pPage->iVLCSize -= pPage->iVLCOff;
        pPage->iVLCOff = 0;
        pPage->bb.pBuf = pPage->ucFileBuf; // reset VLC source pointer too
    }
    if (pPage->JPEGFile.iPos < pPage->JPEGFile.iSize && pPage->iVLCSize < JPEG_FILE_BUF_SIZE-64)
    {
        int i;
        // Try to read enough to fill the buffer
        i = (*pPage->pfnRead)(&pPage->JPEGFile, &pPage->ucFileBuf[pPage->iVLCSize], JPEG_FILE_BUF_SIZE - pPage->iVLCSize); // max length we can read
        // Filter out the markers
        pPage->iVLCSize += JPEGFilter(&pPage->ucFileBuf[pPage->iVLCSize], &pPage->ucFileBuf[pPage->iVLCSize], i, &pPage->ucFF);
    }
} /* JPEGGetMoreData() */

//
// Parse the JPEG header, gather necessary info to decode the image
// Returns 1 for success, 0 for failure
//
static int JPEGParseInfo(JPEGIMAGE *pPage, int bExtractThumb)
{
    int iBytesRead;
    int i, iOffset, iTableOffset;
    uint8_t ucTable, *s = pPage->ucFileBuf;
    uint16_t usMarker, usLen = 0;
    int iFilePos = 0;
    
    if (bExtractThumb) // seek to the start of the thumbnail image
    {
        iFilePos = pPage->iThumbData;
        (*pPage->pfnSeek)(&pPage->JPEGFile, iFilePos);
    }
    iBytesRead = (*pPage->pfnRead)(&pPage->JPEGFile, s, JPEG_FILE_BUF_SIZE);
    if (iBytesRead < 256) // a JPEG file this tiny? probably bad
    {
        pPage->iError = JPEG_INVALID_FILE;
        return 0;
    }
    iFilePos += iBytesRead;
    if (MOTOSHORT(pPage->ucFileBuf) != 0xffd8)
    {
        pPage->iError = JPEG_INVALID_FILE;
        return 0; // not a JPEG file
    }
    iOffset = 2; /* Start at offset of first marker */
    usMarker = 0; /* Search for SOFx (start of frame) marker */
    while (usMarker != 0xffda && iOffset < pPage->JPEGFile.iSize)
    {
        if (iOffset >= JPEG_FILE_BUF_SIZE/2) // too close to the end, read more data
        {
            // Do we need to seek first?
            if (iOffset >= JPEG_FILE_BUF_SIZE)
            {
                iFilePos += (iOffset - iBytesRead);
                iOffset = 0;
                (*pPage->pfnSeek)(&pPage->JPEGFile, iFilePos);
                iBytesRead = 0; // throw away any old data
            }
            // move existing bytes down
            if (iOffset)
            {
                memcpy(pPage->ucFileBuf, &pPage->ucFileBuf[iOffset], iBytesRead - iOffset);
                iBytesRead -= iOffset;
                iOffset = 0;
            }
            i = (*pPage->pfnRead)(&pPage->JPEGFile, &pPage->ucFileBuf[iBytesRead], JPEG_FILE_BUF_SIZE-iBytesRead);
            iFilePos += i;
            iBytesRead += i;
        }
        usMarker = MOTOSHORT(&s[iOffset]);
        iOffset += 2;
        usLen = MOTOSHORT(&s[iOffset]); // marker length

        if (usMarker < 0xffc0 || usMarker == 0xffff) // invalid marker, could be generated by "Arles Image Web Page Creator" or Accusoft
        {
            iOffset++;
            continue; // skip 1 byte and try to resync
        }
        switch (usMarker)
        {
            case 0xffc1:
            case 0xffc2:
            case 0xffc3:
                pPage->iError = JPEG_UNSUPPORTED_FEATURE;
                return 0; // currently unsupported modes
                
            case 0xffe1: // App1 (EXIF?)
                if (s[iOffset+2] == 'E' && s[iOffset+3] == 'x' && (s[iOffset+8] == 'M' || s[iOffset+8] == 'I')) // the EXIF data we want
                {
                    int bMotorola, IFD, iTagCount;
                    pPage->iEXIF = iFilePos - iBytesRead + iOffset + 8; // start of TIFF file
                    // Get the orientation value (if present)
                    bMotorola = (s[iOffset+8] == 'M');
                    IFD = TIFFLONG(&s[iOffset+12], bMotorola);
                    iTagCount = TIFFSHORT(&s[iOffset+16], bMotorola);
                    GetTIFFInfo(pPage, bMotorola, IFD+iOffset+8);
                    // The second IFD defines the thumbnail (if present)
                    if (iTagCount >= 1 && iTagCount < 32) // valid number of tags for EXIF data 'page'
                    {
                       // point to next IFD
                        IFD += (12 * iTagCount) + 2;
                        IFD = TIFFLONG(&s[IFD + iOffset + 8], bMotorola);
                        if (IFD != 0) // Thumbnail present?
                        {
                            pPage->ucHasThumb = 1;
                            GetTIFFInfo(pPage, bMotorola, IFD+iOffset+8); // info for second 'page' of TIFF
                            pPage->iThumbData += iOffset + 8; // absolute offset in the file
                        }
                    }
                }
                break;
            case 0xffc0: // SOFx - start of frame
                pPage->ucMode = (uint8_t)usMarker;
                pPage->ucBpp = s[iOffset+2]; // bits per sample
                pPage->iHeight = MOTOSHORT(&s[iOffset+3]);
                pPage->iWidth = MOTOSHORT(&s[iOffset+5]);
                pPage->ucNumComponents = s[iOffset+7];
                pPage->ucBpp = pPage->ucBpp * pPage->ucNumComponents; /* Bpp = number of components * bits per sample */
                if (pPage->ucNumComponents == 1)
                    pPage->ucSubSample = 0; // use this to differentiate from color 1:1
                else
                {
                    usLen -= 8;
                    iOffset += 8;
//                    pPage->ucSubSample = s[iOffset+9]; // subsampling option for the second color component
                    for (i=0; i<pPage->ucNumComponents; i++)
                    {
                        uint8_t ucSamp;
                        pPage->JPCI[i].component_id = s[iOffset++];
                        pPage->JPCI[i].component_index = (unsigned char)i;
                        ucSamp = s[iOffset++]; // get the h+v sampling factor
                        if (i == 0) // Y component?
                            pPage->ucSubSample = ucSamp;
//                        pPage->JPCI[i].h_samp_factor = ucSamp >> 4;
//                        pPage->JPCI[i].v_samp_factor = ucSamp & 0xf;
                        pPage->JPCI[i].quant_tbl_no = s[iOffset++]; // quantization table number
                        usLen -= 3;
                    }
                }
                break;
            case 0xffdd: // Restart Interval
                if (usLen == 4)
                    pPage->iResInterval = MOTOSHORT(&s[iOffset+2]);
                break;
            case 0xffc4: /* M_DHT */ // get Huffman tables
                iOffset += 2; // skip length
                usLen -= 2; // subtract length length
                if (JPEGGetHuffTables(&s[iOffset], usLen, pPage) != 0) // bad tables?
                {
                    pPage->iError = JPEG_DECODE_ERROR;
                    return 0; // error
                }
                break;
            case 0xffdb: /* M_DQT */
                /* Get the quantization tables */
                /* first byte has PPPPNNNN where P = precision and N = table number 0-3 */
                iOffset += 2; // skip length
                usLen -= 2; // subtract length length
                while (usLen > 0)
                {
                    ucTable = s[iOffset++]; // table number
                    if ((ucTable & 0xf) > 3) // invalid table number
                    {
                        pPage->iError = JPEG_DECODE_ERROR;
                        return 0;
                    }
                    iTableOffset = (ucTable & 0xf) * DCTSIZE;
                    if (ucTable & 0xf0) // if word precision
                    {
                        for (i=0; i<DCTSIZE; i++)
                        {
                            pPage->sQuantTable[i+iTableOffset] = MOTOSHORT(&s[iOffset]);
                            iOffset += 2;
                        }
                        usLen -= (DCTSIZE*2 + 1);
                    }
                    else // byte precision
                    {
                        for (i=0; i<DCTSIZE; i++)
                        {
                            pPage->sQuantTable[i+iTableOffset] = (unsigned short)s[iOffset++];
                        }
                        usLen -= (DCTSIZE + 1);
                    }
                }
                break;
        } // switch on JPEG marker
        iOffset += usLen;
    } // while
    if (usMarker == 0xffda) // start of image
    {
        if (pPage->ucBpp != 8) // need to match up table IDs
        {
            iOffset -= usLen;
            JPEGGetSOS(pPage, &iOffset); // get Start-Of-Scan info for decoding
        }
        if (!JPEGMakeHuffTables(pPage, 0)) //int bThumbnail) DEBUG
        {
            pPage->iError = JPEG_UNSUPPORTED_FEATURE;
            return 0;
        }
        // Now the offset points to the start of compressed data
        i = JPEGFilter(&pPage->ucFileBuf[iOffset], pPage->ucFileBuf, iBytesRead-iOffset, &pPage->ucFF);
        pPage->iVLCOff = 0;
        pPage->iVLCSize = i;
        JPEGGetMoreData(pPage); // read more VLC data
        return 1;
    }
    pPage->iError = JPEG_DECODE_ERROR;
    return 0;
} /* JPEGParseInfo() */
//
// Fix and reorder the quantization table for faster decoding.*
//
static void JPEGFixQuantD(JPEGIMAGE *pJPEG)
{
    int iTable, iTableOffset;
    signed short sTemp[DCTSIZE];
    int i;
    uint16_t *p;
    
    for (iTable=0; iTable<pJPEG->ucNumComponents; iTable++)
    {
        iTableOffset = iTable * DCTSIZE;
        p = (uint16_t *)&pJPEG->sQuantTable[iTableOffset];
        for (i=0; i<DCTSIZE; i++)
            sTemp[i] = p[cZigZag[i]];
        memcpy(&pJPEG->sQuantTable[iTableOffset], sTemp, DCTSIZE*sizeof(short)); // copy back to original spot
        
        // Prescale for DCT multiplication
        p = (uint16_t *)&pJPEG->sQuantTable[iTableOffset];
        for (i=0; i<DCTSIZE; i++)
        {
            p[i] = (uint16_t)((p[i] * iScaleBits[i]) >> 12);
        }
    }
} /* JPEGFixQuantD() */
//
// Decode the 64 coefficients of the current DCT block
//
static int JPEGDecodeMCU(JPEGIMAGE *pJPEG, int iMCU, int *iDCPredictor)
{
    uint32_t ulCode, ulTemp;
    uint8_t *pZig;
    signed char cCoeff;
    unsigned short *pFast;
    unsigned char ucHuff, *pucFast;
    uint32_t usHuff; // this prevents an unnecessary & 65535 for shorts
    uint32_t ulBitOff, ulBits; // local copies to allow compiler to use register vars
    uint8_t *pBuf, *pEnd, *pEnd2;
    signed short *pMCU = &pJPEG->sMCUs[iMCU];
    uint8_t ucMaxACCol, ucMaxACRow;
    
    #define MIN_DCT_THRESHOLD 8
        
    ulBitOff = pJPEG->bb.ulBitOff;
    ulBits = pJPEG->bb.ulBits;
    pBuf = pJPEG->bb.pBuf;
        
    pZig = (unsigned char *)&cZigZag2[1];
    pEnd = (unsigned char *)&cZigZag2[64];
    
    if (ulBitOff > (REGISTER_WIDTH-17)) // need to get more data
    {
        pBuf += (ulBitOff >> 3);
        ulBitOff &= 7;
        ulBits = MOTOLONG(pBuf);
    }
    if (pJPEG->iOptions & (JPEG_SCALE_QUARTER | JPEG_SCALE_EIGHTH)) // reduced size DCT
    {
        pMCU[1] = pMCU[8] = pMCU[9] = 0;
        pEnd2 = (uint8_t *)&cZigZag2[5]; // we only need to store the 4 elements we care about
    }
    else
    {
        memset(pMCU, 0, 64*sizeof(short)); // pre-fill with zero since we may skip coefficients
        pEnd2 = (uint8_t *)&cZigZag2[64];
    }
    ucMaxACCol = ucMaxACRow = 0;
    pZig = (unsigned char *)&cZigZag2[1];
    pEnd = (unsigned char *)&cZigZag2[64];

    // get the DC component
    pucFast = &pJPEG->ucHuffDC[pJPEG->ucDCTable * DC_TABLE_SIZE];
    ulCode = (ulBits >> (REGISTER_WIDTH - 12 - ulBitOff)) & 0xfff; // get as lower 12 bits
    if (ulCode >= 0xf80) // it's a long code
        ulCode = (ulCode & 0xff); // point to long table and trim to 7-bits + 0x80 offset into long table
    else
        ulCode >>= 6; // it's a short code, use first 6 bits only
    ucHuff = pucFast[ulCode];
    cCoeff = (signed char)pucFast[ulCode+512]; // get pre-calculated extra bits for "small" values
    if (ucHuff == 0) // invalid code
        return -1;
    ulBitOff += (ucHuff >> 4); // add the Huffman length
    ucHuff &= 0xf; // get the actual code (SSSS)
    if (ucHuff) // if there is a change to the DC value
    { // get the 'extra' bits
        if (cCoeff)
        {
            (*iDCPredictor) += cCoeff;
        }
        else
        {
            if (ulBitOff > (REGISTER_WIDTH - 17)) // need to get more data
            {
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            ulCode = ulBits << ulBitOff;
            ulTemp = ~(uint32_t)(((int32_t)ulCode)>>31); // slide sign bit across other 31 bits
            ulCode >>= (REGISTER_WIDTH - ucHuff);
            ulCode -= ulTemp>>(REGISTER_WIDTH-ucHuff);
            ulBitOff += ucHuff; // add bit length
            (*iDCPredictor) += (int)ulCode;
        }
    }
    pMCU[0] = (short)*iDCPredictor; // store in MCU[0]
    // Now get the other 63 AC coefficients
    pFast = &pJPEG->usHuffAC[pJPEG->ucACTable * HUFF11SIZE];
    if (pJPEG->b11Bit) // 11-bit "slow" tables used
    {
//            if (pJPEG->pHuffACFast == pJPEG->huffacFast[1]) // second table
//                pFast = &pJPEG->ucAltHuff[0];
        while (pZig < pEnd)
        {
            if (ulBitOff >(REGISTER_WIDTH - 17)) // need to get more data
            {
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            ulCode = (ulBits >> (REGISTER_WIDTH - 16 - ulBitOff)) & 0xffff; // get as lower 16 bits
            if (ulCode >= 0xf000) // first 4 bits = 1, use long table
                ulCode = (ulCode & 0x1fff);
            else
                ulCode >>= 4; // use lower 12 bits (short table)
            usHuff = pFast[ulCode];
            if (usHuff == 0) // invalid code
                return -1;
            ulBitOff += (usHuff >> 8); // add length
            usHuff &= 0xff; // get code (RRRR/SSSS)
            if (usHuff == 0) // no more AC components
            {
                goto mcu_done;
            }
            if (ulBitOff > (REGISTER_WIDTH - 17)) // need to get more data
            {
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            pZig += (usHuff >> 4);  // get the skip amount (RRRR)
            usHuff &= 0xf; // get (SSSS) - extra length
            if (pZig < pEnd && usHuff) // && piHisto)
            {
                ulCode = ulBits << ulBitOff;
                ulTemp = ~(uint32_t) (((int32_t) ulCode) >> (REGISTER_WIDTH-1)); // slide sign bit across other 63 bits
                ulCode >>= (REGISTER_WIDTH - usHuff);
                ulCode -= ulTemp >> (REGISTER_WIDTH - usHuff);
                ucMaxACCol |= 1<<(*pZig & 7); // keep track of occupied columns
                if (*pZig >= 0x20) // if more than 4 rows used in a col, mark it
                    ucMaxACRow |= 1<<(*pZig & 7); // keep track of the max AC term row
                pMCU[*pZig] = (signed short)ulCode; // store AC coefficient (already reordered)
            }
            ulBitOff += usHuff; // add (SSSS) extra length
            pZig++;
        } // while
    }
    else // 10-bit "fast" tables used
    {
        while (pZig < pEnd)
        {
            if (ulBitOff >(REGISTER_WIDTH - 17)) // need to get more data
            {
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            ulCode = (ulBits >> (REGISTER_WIDTH - 16 - ulBitOff)) & 0xffff; // get as lower 16 bits
            if (ulCode >= 0xfc00) // first 6 bits = 1, use long table
                ulCode = (ulCode & 0x7ff); // (ulCode & 0x3ff) + 0x400;
            else
                ulCode >>= 6; // use lower 10 bits (short table)
            usHuff = pFast[ulCode];
            if (usHuff == 0) // invalid code
                return -1;
            ulBitOff += (usHuff >> 8); // add length
            usHuff &= 0xff; // get code (RRRR/SSSS)
            if (usHuff == 0) // no more AC components
            {
                goto mcu_done;
            }
            if (ulBitOff >(REGISTER_WIDTH - 17)) // need to get more data
            {
                pBuf += (ulBitOff >> 3);
                ulBitOff &= 7;
                ulBits = MOTOLONG(pBuf);
            }
            pZig += (usHuff >> 4);  // get the skip amount (RRRR)
            usHuff &= 0xf; // get (SSSS) - extra length
            if (pZig < pEnd2 && usHuff)
            {
                ulCode = ulBits << ulBitOff;
                ulTemp = ~(uint32_t) (((int32_t) ulCode) >> (REGISTER_WIDTH-1)); // slide sign bit across other 63 bits
                ulCode >>= (REGISTER_WIDTH - usHuff);
                ulCode -= ulTemp >> (REGISTER_WIDTH - usHuff);
                ucMaxACCol |= 1<<(*pZig & 7); // keep track of occupied columns
                if (*pZig >= 0x20) // if more than 4 rows used in a col, mark it
                    ucMaxACRow |= 1<<(*pZig & 7); // keep track of the max AC term row
                pMCU[*pZig] = (signed short)ulCode; // store AC coefficient (already reordered)
            }
            ulBitOff += usHuff; // add (SSSS) extra length
            pZig++;
        } // while
    } // 10-bit tables
mcu_done:
    pJPEG->bb.pBuf = pBuf;
    pJPEG->iVLCOff = (int)(pBuf - pJPEG->ucFileBuf);
    pJPEG->bb.ulBitOff = ulBitOff;
    pJPEG->bb.ulBits = ulBits;
    pJPEG->ucMaxACCol = ucMaxACCol;
    pJPEG->ucMaxACRow = ucMaxACRow; // DEBUG
    return 0;
} /* JPEGDecodeMCU() */
//
// Inverse DCT
//
static void JPEGIDCT(JPEGIMAGE *pJPEG, int iMCUOffset, int iQuantTable, int iACFlags)
{
    int iRow;
    unsigned char ucColMask;
    int iCol;
    signed int tmp6,tmp7,tmp10,tmp11,tmp12,tmp13;
    signed int z5,z10,z11,z12,z13;
    signed int tmp0,tmp1,tmp2,tmp3,tmp4,tmp5;
    signed short *pQuant;
    unsigned char *pOutput;
    unsigned char ucMaxACRow, ucMaxACCol;
    int16_t *pMCUSrc = &pJPEG->sMCUs[iMCUOffset];
    
    ucMaxACRow = (unsigned char)(iACFlags >> 8);
    ucMaxACCol = iACFlags & 0xff;
        
    // my shortcut method appears to violate patent 20020080052
    // but the patent is invalidated by prior art:
    // http://netilium.org/~mad/dtj/DTJ/DTJK04/
    pQuant = &pJPEG->sQuantTable[iQuantTable * DCTSIZE];
    if (pJPEG->iOptions & JPEG_SCALE_QUARTER) // special case
    {
        /* Column 0 */
        tmp4 = pMCUSrc[0] * pQuant[0];
        tmp5 = pMCUSrc[8] * pQuant[8];
        tmp0 = tmp4 + tmp5;
        tmp2 = tmp4 - tmp5;
        /* Column 1 */
        tmp4 = pMCUSrc[1] * pQuant[1];
        tmp5 = pMCUSrc[9] * pQuant[9];
        tmp1 = tmp4 + tmp5;
        tmp3 = tmp4 - tmp5;
        /* Pass 2: process 2 rows, store into output array. */
        /* Row 0 */
        pOutput = (unsigned char *)pMCUSrc; // store output pixels back into MCU
        pOutput[0] = ucRangeTable[(((tmp0 + tmp1)>>5) & 0x3ff)];
        pOutput[1] = ucRangeTable[(((tmp0 - tmp1)>>5) & 0x3ff)];
        /* Row 1 */
        pOutput[2] = ucRangeTable[(((tmp2 + tmp3)>>5) & 0x3ff)];
        pOutput[3] = ucRangeTable[(((tmp2 - tmp3)>>5) & 0x3ff)];
        return;
    }
    // do columns first
    ucColMask = ucMaxACCol | 1; // column 0 must always be calculated
    for (iCol = 0; iCol < 8 && ucColMask; iCol++)
    {
        if (ucColMask & (1<<iCol)) // column has data in it
        {
            ucColMask &= ~(1<<iCol); // unmark this col after use
            if (!(ucMaxACRow & (1<<iCol))) // simpler calculations if only half populated
            {
                // even part
                tmp10 = pMCUSrc[iCol] * pQuant[iCol];
                tmp1 = pMCUSrc[iCol+16] * pQuant[iCol+16]; // get 2nd row
                tmp12 = ((tmp1*106)>>8); // used to be 362 - 1 (256)
                tmp0 = tmp10 + tmp1;
                tmp3 = tmp10 - tmp1;
                tmp1 = tmp10 + tmp12;
                tmp2 = tmp10 - tmp12;
                // odd part
                tmp4 = pMCUSrc[iCol+8] * pQuant[iCol+8]; // get 1st row
                tmp5 = pMCUSrc[iCol+24];
                if (tmp5) // this value is usually 0
                {
                    tmp5 *= pQuant[iCol+24]; // get 3rd row
                    tmp7 = tmp4 + tmp5;
                    tmp11 = (((tmp4 - tmp5) * 362) >> 8);  // 362>>8 = 1.414213562
                    z5 = (((tmp4-tmp5) * 473) >> 8);  // 473>>8 = 1.8477
                    tmp12 = ((-tmp5 * -669)>>8) + z5; // -669>>8 = -2.6131259
                    tmp6 = tmp12 - tmp7;
                    tmp5 = tmp11 - tmp6;
                    tmp10 = ((tmp4 * 277)>>8) - z5; // 277>>8 = 1.08239
                    tmp4 = tmp10 + tmp5;
                }
                else // simpler case when we only have 1 odd row to calculate
                {
                    tmp7 = tmp4;
                    tmp5 = (145*tmp4) >> 8;
                    tmp6 = (217*tmp4) >> 8;
                    tmp4 = (-51*tmp4) >> 8;
                }
                pMCUSrc[iCol] = (short)(tmp0 + tmp7);    // row0
                pMCUSrc[iCol+8] = (short)(tmp1 + tmp6);  // row 1
                pMCUSrc[iCol+16] = (short)(tmp2 + tmp5); // row 2
                pMCUSrc[iCol+24] = (short)(tmp3 - tmp4); // row 3
                pMCUSrc[iCol+32] = (short)(tmp3 + tmp4); // row 4
                pMCUSrc[iCol+40] = (short)(tmp2 - tmp5); // row 5
                pMCUSrc[iCol+48] = (short)(tmp1 - tmp6); // row 6
                pMCUSrc[iCol+56] = (short)(tmp0 - tmp7); // row 7
            }
            else // need to do full column calculation
            {
                // even part
                tmp0 = pMCUSrc[iCol] * pQuant[iCol];
                tmp2 = pMCUSrc[iCol+32]; // get 4th row
                if (tmp2) // 4th row is most likely 0
                {
                    tmp2 = tmp2 * pQuant[iCol+32];
                    tmp10 = tmp0 + tmp2;
                    tmp11 = tmp0 - tmp2;
                }
                else
                {
                    tmp10 = tmp11 = tmp0;
                }
                tmp1 = pMCUSrc[iCol+16] * pQuant[iCol+16]; // get 2nd row
                tmp3 = pMCUSrc[iCol+48]; // get 6th row
                if (tmp3) // 6th row is most likely 0
                {
                    tmp3 = tmp3 * pQuant[iCol+48];
                    tmp13 = tmp1 + tmp3;
                    tmp12 = (((tmp1 - tmp3) * 362) >> 8) - tmp13;  // 362>>8 = 1.414213562
                }
                else
                {
                    tmp13 = tmp1;
                    tmp12 = ((tmp1*362)>>8) - tmp1;
                }
                tmp0 = tmp10 + tmp13;
                tmp3 = tmp10 - tmp13;
                tmp1 = tmp11 + tmp12;
                tmp2 = tmp11 - tmp12;
                // odd part
                tmp5 = pMCUSrc[iCol+24] * pQuant[iCol+24]; // get 3rd row
                tmp6 = pMCUSrc[iCol+40]; // get 5th row
                if (tmp6) // very likely that row 5 = 0
                {
                    tmp6 = tmp6 * pQuant[iCol+40];
                    z13 = tmp6 + tmp5;
                    z10 = tmp6 - tmp5;
                }
                else
                {
                    z13 = tmp5;
                    z10 = -tmp5;
                }
                tmp4 = pMCUSrc[iCol+8] * pQuant[iCol+8]; // get 1st row
                tmp7 = pMCUSrc[iCol+56]; // get 7th row
                if (tmp7) // very likely that row 7 = 0
                {
                    tmp7 = tmp7 * pQuant[iCol+56];
                    z11 = tmp4 + tmp7;
                    z12 = tmp4 - tmp7;
                }
                else
                {
                    z11 = z12 = tmp4;
                }
                tmp7 = z11 + z13;
                tmp11 = (((z11 - z13) * 362) >> 8);  // 362>>8 = 1.414213562
                z5 = (((z10 + z12) * 473) >> 8);  // 473>>8 = 1.8477
                tmp12 = ((z10 * -669)>>8) + z5; // -669>>8 = -2.6131259
                tmp6 = tmp12 - tmp7;
                tmp5 = tmp11 - tmp6;
                tmp10 = ((z12 * 277)>>8) - z5; // 277>>8 = 1.08239
                tmp4 = tmp10 + tmp5;
                pMCUSrc[iCol] = (short)(tmp0 + tmp7);    // row0
                pMCUSrc[iCol+8] = (short)(tmp1 + tmp6);  // row 1
                pMCUSrc[iCol+16] = (short)(tmp2 + tmp5); // row 2
                pMCUSrc[iCol+24] = (short)(tmp3 - tmp4); // row 3
                pMCUSrc[iCol+32] = (short)(tmp3 + tmp4); // row 4
                pMCUSrc[iCol+40] = (short)(tmp2 - tmp5); // row 5
                pMCUSrc[iCol+48] = (short)(tmp1 - tmp6); // row 6
                pMCUSrc[iCol+56] = (short)(tmp0 - tmp7); // row 7
            } // full calculation needed
        } // if column has data in it
    } // for each column
    // now do rows
    pOutput = (unsigned char *)pMC