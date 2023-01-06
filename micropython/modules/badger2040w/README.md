# Badger 2040 W <!-- omit in toc -->

Badger 2040 W is a Raspberry Pi Pico W powered E Ink badge.

- [Summary](#summary)
  - [Differences between Badger 2040 W and Badger 2040](#differences-between-badger-2040-w-and-badger-2040)
  - [Getting Started](#getting-started)
  - [Constants](#constants)
    - [Screen Size](#screen-size)
    - [E Ink Pins](#e-ink-pins)
    - [Power Pins](#power-pins)
    - [Activity LED Pin](#activity-led-pin)
- [Function Reference](#function-reference)
  - [Basic Drawing Settings](#basic-drawing-settings)
    - [Pen Colour](#pen-colour)
    - [Pen Thickness](#pen-thickness)
  - [Displaying Images](#displaying-images)
  - [Updating The Display](#updating-the-display)
    - [Update](#update)
    - [Clear](#clear)
    - [Partial Update](#partial-update)
    - [Update Speed](#update-speed)
  - [LED](#led)
  - [Buttons](#buttons)
  - [Waking From Sleep](#waking-from-sleep)
    - [Button Presses](#button-presses)
    - [Real-time Clock](#real-time-clock)
  - [Update Speed](#update-speed-1)
  - [System speed](#system-speed)

# Summary

## Differences between Badger 2040 W and Badger 2040

Badger 2040 W switches from the Badger-specific drawing library of Badger 2040, to our generic PicoGraphics library.

PicoGraphics brings some great improvements, such as JPEG support with dithering and cross-compatibility between all of our other display products.

We've tried to make the transition as simple as possible, but there are a few breaking changes you'll need to be aware of:

* `pen()` is now `set_pen()`
* `update_speed()` is now `set_update_speed()`
* `thickness()` is now `set_thickness()` and *only* applies to Hershey fonts
* `image()` and `icon()` are deprecated, use JPEGs instead.
* `invert()` is not supported.

See the [PicoGraphics function reference](../picographics/README.md) for more information on how to draw to the display.

Additionally Badger 2040 W does not have a "user" button since the BOOTSEL button (which originally doubled as "user") is now aboard the attached Pico W.

## Getting Started

:warning: If you're using the examples-included firmware you're good to go, otherwise you'll need to copy `examples/badger2040w/lib/badger2040w.py`, `micropython/examples/badger2040w/WIFI_CONFIG.py`and `examples/badger2040w/lib/network_manager.py` over to your Badger 2040 W.

To start coding your Badger 2040 W, you will need to add the following lines of code to the start of your code file.

```python
import badger2040w
badger = badger2040w.Badger2040W()
```

This will create a `Badger2040W` class called `badger` that will be used in the rest of the examples going forward.

## Constants

Below is a list of other constants that have been made available, to help with the creation of more advanced programs.

### Screen Size

* `WIDTH` = `296`
* `HEIGHT` = `128`

### E Ink Pins

* `BUSY` = `26`

### Power Pins

* `ENABLE_3V3` = `10`

### Activity LED Pin

* `LED` = `22`

# Function Reference

## Basic Drawing Settings

Since Badger 2040 W is based upon PicoGraphics you should read the [PicoGraphics function reference](../picographics/README.md) for more information about how to draw to the display.

### Pen Colour

There are 16 pen colours - or "shades of grey" - to choose, from 0 (black) to 15 (white).

Since Badger 2040 W cannot display colours other than black and white, any value from 1 to 14 will apply dithering when drawn, to simulate a shade of grey.

```python
set_pen(
    colour  # int: colour from 0 to 15
)
```

### Pen Thickness

:warning: Applies to Hershey font