# Pico Inky Frame MicroPython Examples <!-- omit in toc -->

- [PicoGraphics](#picographics)
- [Examples](#examples)
  - [Button Test](#button-test)
  - [Daily Activity](#daily-activity)
  - [Image Gallery](#image-gallery)
  - [LED PWM](#led-pwm)
  - [News](#news)
  - [PlaceKitten](#placekitten)
  - [Quote of the Day](#quote-of-the-day)
  - [Random Joke](#random-joke)
  - [SD Card Test](#sd-card-test)
  - [XKCD Daily](#xkcd-daily)
  - [Dithering](#dithering)

## PicoGraphics

You can draw on Inky Frame using our tiny PicoGraphics display library.
- [PicoGraphics MicroPython function reference](../../modules/picographics)

## Examples

The wireless examples need `network_manager.py` and `WIFI_CONFIG.py` from the `common` directory to be saved to your Pico W. Open up `WIFI_CONFIG.py` in Thonny to add your wifi details (and save it when you're done).

You'll also need to install the `micropython-urllib.urequest` library using Thonny's 'Tools' > 'Manage Packages' or `common/lib/urllib` which contains a compiled `.mpy` version that uses less RAM. You should place this directory in `lib` on your Pico W.

Finally for examples loading images, you'll need `sdcard.mpy` from `common/lib`. You should place this file in `lib` on your Pico W.

- [/micropython/examples/common](../common)

### Button Test
[button_test.py](button_test.py)

Inky Frame's buttons (and the RTC alarm, busy signal from the screen and external trigger from the hack header) are connected to a shift register to help conserve pins, and to allow these inputs to wake the board up from sleep.

This example demonstrates a simple way of reading when a button has been pushed by reading the shift register and checking if the bit in a specific position is 0 or 1.

### Daily Activity
[inky_frame_daily_activity.py](inky_frame_daily_activity.py)

Generate a random activity from Bored API.

### Image Gallery
[/image_gallery](../inky_frame/image_gallery)

This