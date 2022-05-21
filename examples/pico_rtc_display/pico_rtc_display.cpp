#include <string.h>

// **************************************************************************
// Demonstrate the Pimoroni RTC module (RV3028)
// Assumes that a Pico Display Pack (a 1.14inch IPS LCD screen with four
// useful buttons) is installed, and that the RV3028 I2C module has
// sda, scl and int on GPIO 20, 21 and 22
// Displays a clock showing date and time (no code yet to set the time,
// so do it on a separate machine), or a countdown timer.
// Button A selects the timer setting function, and runs the timer once set
// Button B selects the clock (the selection on startup)
// Button X increments the timer delay in seconds
// Button Y decrements it
// (There are on-screen reminders of the active buttons)
// **************************************************************************

// To use PicoExplorer rather than PicoDisplay, uncomment the following line
// #define USE_PICO_EXPLORER 1
// This:
// - Includes pico_explorer.hpp rather than display.hpp
// - Replaces all PicoDisplay references with PicoExplorer
// - Leaves out the .set_led() calls in flash_led()
#ifdef USE_PICO_EXPLORER
#include "pico_explorer.hpp"
#else
#include "pico_display.hpp"
#endif
#include "breakout_rtc.hpp"

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/button/button.hpp"
#include "drivers/rgbled/rgbled.hpp"

#define MODE_DISP_CLOCK 0
#define MODE_DISP_TIMER 1
#define MODE_SET_TIMER  2

#define DEFAULT_TIMER_COUNT 5

using namespace pimoroni;

#ifdef USE_PICO_EXPLORER
uint16_t screen_width = PicoExplorer::WIDTH;
uint16_t screen_height = PicoExplorer::HEIGHT;

Button button_a(PicoExplorer::A);
Button button_b(PicoExplorer