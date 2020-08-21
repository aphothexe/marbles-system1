#include <math.h>
#include <cfloat>
#include <climits>
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "encoder.hpp"
#include "encoder.pio.h"

#define LAST_STATE(state)  ((state) & 0b0011)
#define CURR_STATE(state)  (((state) & 0b1100) >> 2)

namespace encoder {

////////////////////////////////////////////////////////////////////////////////////////////////////
// STATICS
////////////////////////////////////////////////////////////////////////////////////////////////////
Encoder* Encoder::encoders[][NUM_PIO_STATE_MACHINES] = { { nullptr, nullptr, nullptr, nullptr }, { nullptr, nullptr, nullptr, nullptr } };
uint8_t Encoder::claimed_sms[] = { 0x0, 0x0 };
uint Encoder::pio_program_offset[] = { 0, 0 };


Encoder::Capture::Capture()
: captured_count(0), captured_delta(0), captured_frequency(0.0f), counts_per_rev(INT32_MAX) {
}

Encoder::Capture::Capture(int32_t count, int32_t delta, float frequency, float counts_per_rev)
: captured_count(count), captured_delta(delta), captured_frequency(frequency)
, counts_per_rev(MAX(counts_per_rev, FLT_EPSILON)) { //Clamp counts_per_rev to avoid potential NaN
}

int32_t Encoder::Capture::count() const {
  return captured_count;
}

int32_t Encoder::Capture::delta() const {
  return captured_delta;
}

float Encoder::Capture::frequency() const {
  return captured_frequency;
}

float Encoder::Capture::revolutions() const {
  return (float)captured_count / counts_per_rev;
}

float Encoder::Capture::degrees() const {
  return revolutions() * 360.0f;
}

float Encoder::Capture::radians() const {
  return revolutions() * M_TWOPI;
}

float Encoder::Capture::revolutions_delta() const {
  return (float)captured_delta / counts_per_rev;
}

float Encoder::Capture::degrees_delta() const {
  return revolutions_delta() * 360.0f;
}

float Encoder::Capture::radians_delta() const {
  return revolutions_delta() * M_TWOPI;
}

float Encoder::Capture::revolutions_per_second() const {
  return captured_frequency / counts_per_rev;
}

float Encoder::Capture::revolutions_per_minute() const {
  return revolutions_per_second() * 60.0f;
}

float Encoder::Capture::degrees_per_second() const {
  return revolutions_per_second() * 360.0f;
}

float Encoder::Capture::radians_per_second() const {
  return revolutions_per_second() * M_TWOPI;
}

Encoder::Encoder(PIO pio, uint sm, const pin_pair &pins, uint common_pin, Direction direction,
                  float counts_per_rev, bool count_microsteps, uint16_t freq_divider)
: pio(pio)
