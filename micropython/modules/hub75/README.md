# HUB75 <!-- omit in toc -->

This library can be used with the Interstate 75 and 75W bypasses the use of picographics and can be used for situations where RAM is constrained or for custom display configurations like 2x2 HUB75 panels.

For most cases we recommend using the picographics based module for Interstate 75 and 75W as it contains a lot of helper functions to draw text and shapes, further information on its usage can be found here: [Interstate75](../../modules_py/interstate75.md) 

The Interstate 75 library is intended for the Interstate 75 and Interstate 75 W "HUB75" matrix panel driver board.

It can, in theory, be used with your own custom wiring, though custom pin assignments are not supported yet.

- [Notes On PIO & DMA Limitations](#notes-on-pio--dma-limitations)
- [Getting Started](#getting-started)
  - [FM6216A Panels](#fm6216a-panels)
  - [Setting Colour Order](#setting-colour-order)
- [Quick Reference](#quick-reference)
  - [Set A Pixel](#set-a-pixel)
  - [Clear The Display](#clear-the-display)

## Notes On PIO & DMA Limitations

The Hub 75 driver uses the PIO hardware on the RP2040. There are only two PIOs with four state machines each, and hub75 uses one of these (`PIO0`) and two state machines- one for clocking out pixels, and another for latching/lighting a row.

It also uses two DMA channels, one to copy pixel data from the back buffer back to the front buffer and one to supply the row driving PIO with row data.

## Getting Started

Const