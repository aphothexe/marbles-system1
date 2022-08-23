# Pico Graphics <!-- omit in toc -->

Pico Graphics is a tiny graphics library supporting a number of underlying buffer formats including 8-bit paletted (256 colour), 8-bit RGB332 (256 colour), 16-bit RGB565 (65K colour) and 4-bit packed (8 colour).

It supports drawing text, primitive and individual pixels and includes basic types such as `Rect` and `Point` brimming with methods to help you develop games and applications.

- [Overview](#overview)
  - [Pen Types](#pen-types)
  - [Creating A Pico Graphics Instance](#creating-a-pico-graphics-instance)
- [Function Reference](#function-reference)
  - [Types](#types)
    - [Rect](#rect)
      - [Rect.empty](#rectempty)
      - [Rect.contains](#rectcontains)
      - [Rect.intersects](#rectintersects)
      - [Rect.intersection](#rectintersection)
      - [Rect.inflate & Rect.deflate](#rectinflate--rectdeflate)
    - [Point](#point)
      - [Point.clamp](#pointclamp)
  - [Pens & Clipping](#pens--clipping)
    - [set_pen](#set_pen)
    - [create_pen](#create_pen)
    - [set_clip & remove_clip](#set_clip--remove_clip)
  - [Palette](#palette)
    - [update_pen](#update_pen)
    - [reset_pen](#reset_pen)
  - [Pixels](#pixels)
    - [pixel](#pixel)
    - [pixel_span](#pixel_span)
  - [Primitives](#primitives)
    - [rectangle](#rectangle)
    - [circle](#circle)
  - [Text](#text)
  - [Change Font](#change-font)


## Overview

Pico Graphics comes in multiple flavours depending on which underlying buffer type you wish to work with.

Your buffer doesn't have to be native to your display. For example a 16-bit ST7789 display can work with P4, P8, RGB332 and RGB565 buffers, with palette lookups handled for you on the fly.

A monochrome OLED display can currently only work with `1Bit` type buffers, and Inky Frame only with `3Bit`.

### Pen Types

* `1Bit` and `1BitY` - 1-bit packed, with automatic dithering from 16 shades of grey. 0 == Black, 15 == White. (For Inky Pack, or monochrome OLEDs)
* `3Bit` - 3-bit bitplaned, using three 1-bit buffers and supporting up to 8 colours. (For Inky Frame)
* `P4` - 4-bit packed, with an 8 colour palette. This is commonly used for 7/8-colour e-ink displays or driving large displays with few colours.
* `P8` - 8-bit, with a 256 colour palette. Great balance of memory usage versus available colours. You can replace palette entries on the fly.
* `RGB332` - 8-bit, with a fixed 256 colour RGB332 palette. Great for quickly porting an RGB565 app to use less RAM. Limits your colour choices, but is easier to grok.
* `RGB565` - 16-bit, 65K "High Colour." Great for rainbows, gradients and images but comes at the cost of RAM!
* `RGB888` - 24-bit, 16M "True Colour." Lists every color possible to be seen by the human eye but comes at double the cost of RAM compared to `RGB565`.

### Creating A Pico Graphics Instance

To create a Pico Graphics instance to draw into, you should construct an instance of the Pen type class you want to use:

```c++
PicoGraphics_Pen3Bit graphics(WIDTH, HEIGHT, nullptr);   // For Inky Frame
PicoGraphics_Pen1Bit graphics(WIDTH, HEIGHT, nullptr);   // For MonoChrome OLEDs
PicoGraphics_Pen1BitY graphics(WIDTH, HEIGHT, nullptr);  // For Inky Pack / Badger 2040
PicoGraphics_PenP4 graphics(WIDTH, HEIGHT, nullptr);     // For colour LCDs such as Pico Display
PicoGraphics_PenP8 graphics(WIDTH, HEIGHT, nullptr);     // ditto- uses 2x the RAM of P4
PicoGraphics_PenRGB332 graphics(WIDTH, HEIGHT, nullptr); // ditto
PicoGraphics_PenRGB565 graphics(WIDTH, HEIGHT, nullptr); // For 16-bit colour LCDs. Uses 2x the RAM of P8 or RGB332 but permits 65K colour.
PicoGraphics_PenRGB8