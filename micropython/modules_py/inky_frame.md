# Inky Frame (MicroPython) <!-- omit in toc -->

Most of your interaction with Inky Frame will be via the PicoGraphics library, but we have created an `inky_frame` module to help you read the onboard A, B, C, D and E buttons and control the LEDs.

- [Pico Graphics](#pico-graphics)
  - [Colour \& Dithering](#colour--dithering)
  - [Images \& JPEGs](#images--jpegs)
- [Buttons \& LEDs](#buttons--leds)
  - [Status LEDs](#status-leds)
- [Battery Power \& RTC](#battery-power--rtc)
  - [Function Reference](#function-reference)
    - [Wakeup States](#wakeup-states)
    - [RTC](#rtc)
    - [Other](#other)
- [RAM Usage](#ram-usage)
  - [Accessing The Framebuffer](#accessing-the-framebuffer)


## Pico Graphics

You can draw on Inky Frame using our PicoGraphics display library.
- [PicoGraphics MicroPython function reference](../../modules