from picographics import PicoGraphics, DISPLAY_TUFTY_2040, PEN_P4
from pimoroni import Button
import math
import time

"""
Tufty2040: Solarized

Sets up Tufty 2040 in 4-bit colour mode and fills the
16-bit palette with some nice Solarized colours.

See: https://ethanschoonover.com/solarized/
"""


# Background & Content Tones
BASE_03 = 0
BASE_02 = 1
BASE_01 = 2
BASE_00 = 3
BASE_0 = 4
BASE_1 = 5
BASE_2 = 6
BASE_3 = 7

# Accent Colours
YELLOW = 8
ORANGE = 9
RED = 10
MAGENTA = 11
VIOLET = 12
BLUE = 13
CYAN = 14
GREEN = 15

# Friendly names for light/dark backgrounds
BG_DARK = BASE_03
BG_LIGHT = BASE_2


def bargraph(ox, oy, w, h, value, colors, pips=32):
    step = int(pips / len(colors))
    filled = int(value * pips)
    pip_width = int(w / pips)
    for i in range(pips):
        p = colors[int(i / step)]
        lcd.set_pen(p)
        x = i * pip_width
        if i > filled:
            lcd.rectangle(ox + x, oy + h - 1, pip_width - 2, 1)
        else:
            lcd.rectangle(ox 