# Automatic brightness example.

import time
from machine import ADC, Pin
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040
import micropython

# Constants for automatic brightness adjustment.
# Below about 3/8ths, the backlight is entirely off. The top of the range is OK.
BACKLIGHT_LOW = micropython.const(0.375)
BACKLIGHT_HIGH = micropython.const(1.0)

# The luminance sensor seems to cover the whol