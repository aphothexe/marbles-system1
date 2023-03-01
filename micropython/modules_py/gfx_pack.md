# Pico GFX Pack (MicroPython) <!-- omit in toc -->

This library offers convenient functions for interacting with [Pico GFX Pack](https://shop.pimoroni.com/products/gfxpack) - The Pico GFX Pack adds a 128x64 LCD Matrix display to your headered Raspberry Pi Pico or PicoW, with RGBW backlight and 5 input buttons for all your display and control needs.

## Table of Content
- [Table of Content](#table-of-content)
- [GFX Pack Class](#gfx-pack-class)
  - [Switches](#switches)
  - [RGBW Backlight](#rgbw-backlight)
- [Display](#display)
  - [Backlight](#backlight)


## GFX Pack Class

The `GfxPack` class deals with RGBW backlight and buttons on the GFX Pack. To create one, import the `gfx_pack` module, then define a new `board` variable:

```python
import gfx_pack

board = gfx_pack.GfxPack()
```

From here, all features can be accessed by calling functions on `board`. In addition, when using Qwiic / Stemma QT devices, the I2C channel to use can be accessed with `board.i2c`.

### Switches

GFX Pack has five switches just below the display. To read one of the switches, call `.switch_pressed(switch)`, whe