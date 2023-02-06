# Galactic Unicorn (MicroPython) <!-- omit in toc -->

Galactic Unicorn offers 53x11 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

You can buy one here: https://shop.pimoroni.com/products/galactic-unicorn

## These are not your everyday RGB LEDs!

Internally Galactic Unicorn applies gamma correction to the supplied image data and updates the display with 14-bit precision resulting in extremely linear visual output - including at the low end.

The display is refreshed around 300 times per second (300fps!) allowing for rock solid stability even when being filmed, no smearing or flickering even when in motion.

No strobing or brightness stepping here folks - it's the perfect backdrop for your tricked out streaming setup!

## Getting started

The Galactic Unicorn library provides a collection of methods that allow you to easily access all of the features on the board.

Drawing is primarily handled via our [PicoGraphics](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/picographics) library which provides a comprehensive selection of drawing methods - once your drawing work is complete you pass the PicoGraphics object to Galactic Unicorn to have it displayed on the screen.

- [Example Program](#example-program)
- [Interleaved Framebuffer](#interleaved-framebuffer)
- [Function Reference](#function-reference)
  - [Imports and Objects](#imports-and-objects)
  - [System State](#system-state)
    - [`set_brightness(value)`](#set_brightnessvalue)
    - [`get_brightness()`](#get_brightness)
    - [`adjust_brightness(delta)`](#adjust_brightnessdelta)
    - [`set_volume(value)`](#set_volumevalue)
    - [`get_volume()`](#get_volume)
    - [`adjust_volume(delta)`](#adjust_volumedelta)
    - [`light()`](#light)
    - [`is_pressed(button)`](#is_pressedbutton)
  - [Drawing](#drawing)
    - [`update(PicoGraphics)`](#updatepicographics)
    - [`clear()`](#clear)
  - [Audio](#audio)
    - [`play_sample(data)`](#play_sampledata)
    - [`synth_channel(channel)`](#synth_channelchannel)
    - [`play_synth()`](#play_synth)
    - [`stop_playing()`](#stop_playing)
    - [Channel Reference](#channel-reference)
  - [Constants](#constants)
    - [`WIDTH` & `HEIGHT`](#width--height)
  - [Using Breakouts](#using-breakouts)

# Example Program

The following example shows how to scroll a simple message across the display.

```python
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN
import time

# create a PicoGraphics framebuffer to draw into
graphics = PicoGraphics(display=DISPLAY_GALACTIC_UNICORN)

# create our GalacticUnicorn object
gu = GalacticUnicorn()

# start position for scrolling (off the side of the display)
scroll = float(-GalacticUnicorn.WIDTH)

# message to scroll
MESSAGE = "Pirate. Monkey. Robot. Ninja."

# pen colours to draw with
BLACK = graphics.create_pen(0, 0, 0)
YELLOW = graphics.create_pen(255, 255, 0)

while True:
    # determine the scroll position of the text
    width = graphics.measure_text(MESSAGE, 1)
    scroll += 0.25
    if scroll > width:
      scroll = float(-GalacticUnicorn.WIDTH)

    # clear the graphics object
    graphics.set_pen(BLACK)
    graphics.clear()

    # draw the text
    graphics.set_pen(YELLOW)
    graphics.text(MESSAGE, round(0 - scroll), 2, -1, 0.55);    

    # update the display
    gu.update(graphics)

    time.sleep(0.02)
```

# Interleaved Framebuffer

Galactic Unicorn takes advantage of the RP2040's PIOs to drive screen updates - this is what gives it the performance it needs to render with 14-bit precision at over 300 frames per second.

The PIO is a powerful, but limited, tool. It has no way to access memory at random and minimal support for decision making and branching. All it can really do is process a stream of data/instructions in order. 

This means that we need to be clever about the way we pass data into the PIO program, the information needs to be delivered in the exact order that the PIO will need to process it. To achieve this we "interleave" our framebuffer - each frame of BCM data is passed one after another with values for the current row, pixel count, and timing inserted as needed:

    row 0 data:
      for each bcd frame:
        bit    : data
              0: 00110110                           // row pixel count (minus one)
        1  - 53: xxxxxbgr, xxxxxbgr, xxxxxbgr, ...  // pixel data
        54 - 55: xxxxxxxx, xxxxxxxx                 // dummy bytes to dword align
             56: xxxxrrrr                           // row select bits
        57 - 59: tttttttt, tttttttt, tttttttt       // bcd tick count (0-65536)
    
    row 1 data:
      ...

If you're working with our library then you don't need to worry about any of these details, they are handled for you.

# Function Reference

## Imports and Objects

To access these functions, you'll need to first `import` the relevant libraries and then set up a Galactic Unicorn object:

```python
from galactic import GalacticUnicorn

gu = GalacticUnicorn()
```

or (with PicoGraphics):

```python
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN

gu = GalacticUnicorn()
graphics = PicoGraphics(display=DISPLAY_GALACTIC_UNICORN)
```

## System State

### `set_brightness(value)`

Set the brightness - `value` is supplied as a floating point value between `0.0` a