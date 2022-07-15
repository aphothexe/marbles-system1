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
    22725, 31521, 29692, 26722