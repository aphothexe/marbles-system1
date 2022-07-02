# Galactic Unicorn (C/C++)<!-- omit in toc -->

Galactic Unicorn offers 53x11 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

You can buy one here: https://shop.pimoroni.com/products/galactic-unicorn

## These are not your everyday RGB LEDs!

Internally Galactic Unicorn applies gamma correction to the supplied image data and updates the display with 14-bit precision resulting in extremely linear visual output - including at the low end.

The display is refreshed around 300 times per second (300fps!) allowing for rock solid stability even when being filmed, no smearing or flickering even when in motion.

No strobing or brightness stepping here folks - it's the perfect backdrop for your tricked out streaming setup!

## Getting started

The Galactic Unicorn library provides a collection of methods that allow you to easily access all of the features on the board.

Drawing is primarily handled via our [PicoGraphics](https://github.com/pimoroni/pimoroni-pico/tree/main/libraries/pico_graphics) library which provides a comprehensive selection of drawing methods - once your drawing work is complete you pass the PicoGraphics object to Galactic Unicorn to have it displayed on the screen.

- [Example Program](#example-program)
- [Interleaved Framebuffer](#interleaved-framebuffer)
- [Function Reference](#function-reference)
  - [System State](#system-state)
    - [`void init()`](#void-init)
    - [`void set_brightness(float value)`](#void-set_brightnessfloat-value)
    - [`float get_brightness()`](#float-get_brightness)
    - [`void adjust_brightness(float delta)`](#void-adjust_brightnessfloat-delta)
    - [`void set_volume(float value)`](#void-set_volumefloat-value)
    - [`float get_volume()`](#float-get_volume)
    - [`void adjust_volume(float delta)`](#void-adjust_volumefloat-delta)
    - [`uint16_t light()`](#uint16_t-light)
    - [`bool is_pressed(uint8_t button)`](#bool-is_presseduint8_t-button)
  - [Drawing](#drawing)
    - [`void update(PicoGraphics *graphics)`](#void-updatepicographics-graphics)
    - [`void clear()`](#void-clear)
    - [`void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)`](#void-set_pixelint-x-int-y-uint8_t-r-uint8_t-g-uint8_t-b)
  - [Audio](#audio)
    - [`void play_sample(uint8_t *data, uint32_t length)`](#void-play_sampleuint8_t-data-uint32_t-length)
    - [`AudioChannel& synth_channel(uint channel)`](#audiochannel-synth_channeluint-channel)
    - [`void play_synth()