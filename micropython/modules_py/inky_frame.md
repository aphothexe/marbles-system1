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
- [PicoGraphics MicroPython function reference](../../modules/picographics)

### Colour & Dithering

Inky Frame supports eight native colours, those that it's physically capable of displaying. These are:

* `BLACK` = 0
* `WHITE` = 1
* `GREEN` = 2
* `BLUE` = 3
* `RED` = 4
* `YELLOW` = 5
* `ORANGE` = 6
* `TAUPE` = 7 (only used for dither on 4.0" and 5.7" displays †)

:info: † - the "taupe" colour (also known as clear) is the colour used to reset the display to a default, clear state. We misuse this as an extra colour on 4.0" and 5.7", but the 7.3" display clear colour is a sort of muddy, greenish gradient that's not consistent enough. You can stil use it, by all means, but it wont be considered for dithering.

These colours are available as constants in the `inky_frame` module so you don't have to remember which number corresponds to which, eg:

```python
display.set_pen(inky_frame.YELLOW)
```

You can use colours outside of these eight by using `create_pen(r, g, b)` or `create_pen_hsv(h, s, v)`. eg:

```python
my_colour = display.create_pen(255, 128, 0)
display.set_pen(my_colour)
```

PicoGraphics will do its best to match your colour choice by mixing the available colours with a dither pattern. This works best on large areas of colour such as backgrounds, big UI elements or chunky text. See the `inky_frame_dithering.py` example for a demonstration.

:info: Due to the lack of light blue or pink native colours, Inky Frame isn't very good at reproducing pink, purple, violet or light blue, cyan and teal.


### Images & JPEGs

You can use the `jpegdec` (JPEG decoding) module to display JPEG files on Inky, but you should be aware of some caveats:

1. JPEGs are compressed and lossy, a small JPEG displayed on Inky might show random specks of colour where you don't expect them as PicoGraphics tries its best to dither noise to the nearest available colours.
2. JPEGs are, by default, dithered to the 7 (or 8 on 4.0 and 5.7) available colours. This uses "ordered dither," which is fast, but not very pretty.
3. You can turn off dithering with `jpeg.decode(dither=False)` for a posterised effect.

## Buttons & LEDs

Inky Frame has five user buttons labelled A to E, with LEDs. The buttons are connected via a shift-register and available in the `inky_frame` module as `button_a`, `button_b`, `button_c`, `button_d` and `button_e`.

Each button has some convenient methods for checking if it's pressed:

* `raw()` - get the current state of the button with no debounce (returns `True` the first time it's called if the corresponding button)
* `read()` - read the current button state, observing debounce (50ms)

Additionally each button has some methods for controlling its associated LED:

* `led_on()` - turn the LED on
* `led_off()` - turn the LED off
* `led_toggle()` - toggle the LED
* `led_brightness(0.5)` - set the LED brightness (from 0.0 to 1.0)

### Status LEDs

In addition to the button LEDs there are two status LEDs, `busy` and `wifi`, which are available as:

* `led_busy` - connected to pin `LED_BUSY`
* `led_wifi` - connected to pin `LED_WIFI`

These are instances of the `pimoroni.PWMLED` class, and have the following methods:

* `on()` - turn the LED on
* `off()` - turn the LED off
* `toggle()` - toggle the LED
* `brightness(0.5)` - set the LED brightness (from 0.0 to 1.0)

:info: `toggle()` is provided for compatibility with the `Pin()` class. It's a little odd, since it changes the LED brightness to `1.0 - current_brightness`.

## Battery Power & RTC

When running on battery power, Inky Frame's buttons cause it to wake from a power-off state and start running your code. Additionally an onboard PCF85063A real-time clock runs continuously from battery and can wake up your Frame automatically.

The Inky Frame library includes a number of convenience functions to set the clock and sleep your device. If you want accurate time you must check if the clock is set - usually a simple check against the year works - connect to a network, and set via NTP (Network Time Protocol).

Inky Frame technically has *two* real-time clocks, the external RTC we've added - which remains continuously powered by battery - and the internal RTC of the P