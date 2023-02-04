
# Cosmic Unicorn (MicroPython) <!-- omit in toc -->

Cosmic Unicorn offers 32x32 bright RGB LEDs driven by Pico W's PIO in addition to a 1W amplifier + speaker, a collection of system and user buttons, and two Qw/ST connectors for adding external sensors and devices. Woha!

You can buy one here: https://shop.pimoroni.com/products/cosmic-unicorn

## These are not your everyday RGB LEDs!

Internally Cosmic Unicorn applies gamma correction to the supplied image data and updates the display with 14-bit precision resulting in extremely linear visual output - including at the low end.

The display is refreshed around 300 times per second (300fps!) allowing for rock solid stability even when being filmed, no smearing or flickering even when in motion.

No strobing or brightness stepping here folks - it's the perfect backdrop for your tricked out streaming setup!

## Getting started

The Cosmic Unicorn library provides a collection of methods that allow you to easily access all of the features on the board.

Drawing is primarily handled via our [PicoGraphics](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/modules/picographics) library which provides a comprehensive selection of drawing methods - once your drawing work is complete you pass the PicoGraphics object to Cosmic Unicorn to have it displayed on the screen.

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
    - [`WIDTH` \& `HEIGHT`](#width--height)
  - [Using Breakouts](#using-breakouts)

# Example Program

The following example shows how to scroll a simple message across the display.

```python
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN
import time

# create a PicoGraphics framebuffer to draw into
graphics = PicoGraphics(display=DISPLAY_COSMIC_UNICORN)

# create our CosmicUnicorn object
cu = CosmicUnicorn()

# start position for scrolling (off the side of the display)
scroll = float(-CosmicUnicorn.WIDTH)

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
      scroll = float(-CosmicUnicorn.WIDTH)

    # clear the graphics object
    graphics.set_pen(BLACK)
    graphics.clear()

    # draw the text
    graphics.set_pen(YELLOW)
    graphics.text(MESSAGE, round(0 - scroll), 2, -1, 0.55);    

    # update the display
    cu.update(graphics)

    time.sleep(0.02)
```

# Interleaved Framebuffer

Cosmic Unicorn takes advantage of the RP2040's PIOs to drive screen updates - this is what gives it the performance it needs to render with 14-bit precision at over 300 frames per second.

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

To access these functions, you'll need to first `import` the relevant libraries and then set up a Cosmic Unicorn object:

```python
from cosmic import CosmicUnicorn

cu = CosmicUnicorn()
```

or (with PicoGraphics):

```python
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN

cu = CosmicUnicorn()
graphics = PicoGraphics(display=DISPLAY_COSMIC_UNICORN)
```

## System State

### `set_brightness(value)`

Set the brightness - `value` is supplied as a floating point value between `0.0` and `1.0`.

### `get_brightness()`

Returns the current brightness as a value between `0.0` and `1.0`.

### `adjust_brightness(delta)`

Adjust the brightness of the display - `delta` is supplied as a floating point value and will be added to the current brightness (and then clamped to the range `0.0` to `1.0`).

For example:

```python
cu.set_brightness(0.5)
cu.adjust_brightness(0.1)  # brightness is now 0.6
cu.adjust_brightness(0.7)  # brightness is now 1.0
cu.adjust_brightness(-0.2)  # brightness is now 0.8
```

### `set_volume(value)`

Set the volume - `value` is supplied as a floating point value between `0.0` and `1.0`.

### `get_volume()`

Returns the current volume as a value between `0.0` and `1.0`.

### `adjust_volume(delta)`

Adjust the volume - `delta` is supplied as a floating point value and will be added to the current volume (and then clamped to the range `0.0` to `1.0`).

For example:

```python
cu.set_volume(0.5)
cu.set_volume(0.1)  # volume is now 0.6
cu.adjust_volume(0.7)  # volume is now 1.0
cu.adjust_volume(-0.2)  # volume is now 0.8
```

### `light()`

Get the current value seen by the onboard light sensor as a value between `0` and `4095`.

### `is_pressed(button)`

Returns true if the requested `button` is currently pressed.

There are a set of constants in the CosmicUnicorn class that represent each of the buttons. The brightness, sleep, and volume buttons are not tied to hardware functions (they are implemented entirely in software) so can also be used for user functions if preferred. Here's a list of the constants and their associated pin numbers:

```python
SWITCH_A               =  0
SWITCH_B               =  1
SWITCH_C               =  3
SWITCH_D               =  6
SWITCH_SLEEP           = 27
SWITCH_VOLUME_UP       =  7
SWITCH_VOLUME_DOWN     =  8
SWITCH_BRIGHTNESS_UP   = 21
SWITCH_BRIGHTNESS_DOWN = 26
```

For example:

```python
while not cu.is_pressed(CosmicUnicorn.SWITCH_A):
    # wait for switch A to be pressed
    pass

print("We did it! We pressed switch A! Heck yeah!")
```

## Drawing

### `update(PicoGraphics)`

The PicoGraphics library provides a collection of powerful drawing methods to make things simple.

The image on the PicoGraphics object provided is copied to the interleaved framebuffer with gamma correction applied.

For example (assuming you've set up your Cosmic Unicorn and PicoGraphics objects up [as we did above](#imports-and-objects)):

```python
cu.update(graphics)
```

⚠️ If you've used PicoGraphics on our other boards note that this `update` function works a little differently. Here it's a Cosmic Unicorn function to which you need to pass a PicoGraphics object to.

### `clear()`

Clear the contents of the interleaved framebuffer. This will make your Cosmic Unicorn display turn off. To show an image again, call the `update()` function as described above.

## Audio

Audio functionality is supported by our [PicoSynth library](https://github.com/pimoroni/pimoroni-pico/tree/main/libraries/pico_synth) which allows you to create multiple voice channels with ADSR (attack decay sustain release) envelopes. It provides a similar set of functionality to the classic SID chip in the Commodore 64.

### `play_sample(data)`

Play the provided 16-bit audio sample. `data` must point to a `bytearray` that contains 16-bit PCM data. The number of samples is retrieved from the array's length.

### `synth_channel(channel)`

Gets a `Channel` object which can then be configured with voice, ADSR envelope, etc.

### `play_synth()`

Start the synth playing.

### `stop_playing()`

Stops any currently playing audio.

### Channel Reference

```python
configure(waveforms=None, frequency=None, volume=None,
          attack=None, decay=None, sustain=None,
          release=None, pulse_width=None)
restore()
waveforms()
waveforms(waveforms)
frequency()
frequency(frequency)
volume()
volume(volume)
attack_duration()
attack_duration(duration)
decay_duration()
decay_duration(duration)
sustain_level()
sustain_level(level)
release_duration()
release_duration(duration)
pulse_width()
pulse_width(width)
trigger_attack() # start the channel playing
trigger_release() # stop the channel playing
play_tone(frequency, volume=None, attack=None, release=None)
```

## Constants

### `WIDTH` & `HEIGHT`

The width and height of Cosmic Unicorn are available in constants `WIDTH` and `HEIGHT`.

For example:

```python
num_pixels = CosmicUnicorn.WIDTH * CosmicUnicorn.HEIGHT
print(num_pixels)
```

## Using Breakouts

Cosmic Unicorn has two Qw/ST (Qwiic/STEMMA QT) connectors. Breakouts with Qw/ST connectors, can be plugged straight in with a [JST-SH to JST-SH cable](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609813587). You can connect I2C Breakout Garden breakouts without Qw/ST connectors using a [JST-SH to JST-SH cable](https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609813587) and a [Qw/ST to Breakout Garden adaptor](https://shop.pimoroni.com/products/stemma-qt-qwiic-to-breakout-garden-adapter).

- [List of breakouts currently supported in our C++/MicroPython build](https://github.com/pimoroni/pimoroni-pico#breakouts)

Cosmic Unicorn uses GP4 and GP5 for its I2C interface. You can use the constants in the shared `pimoroni` module to set up the I2C interface:

```python
from pimoroni_i2c import PimoroniI2C
from pimoroni import BREAKOUT_GARDEN_I2C_PINS

i2c = PimoroniI2C(**BREAKOUT_GARDEN_I2C_PINS)
```

Alternatively, you can specify the pin numbers directly:

```python
from pimoroni_i2c import PimoroniI2C

i2c = PimoroniI2C(sda=4, scl=5)
```