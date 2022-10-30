import gc
import time
import random
from galactic import GalacticUnicorn
from picographics import PicoGraphics, DISPLAY_GALACTIC_UNICORN, PEN_P8
from ulab import numpy

"""
A random, trippy effect.
Experiment with the damping, number of spawns, intensity and offset to change the effect.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

gu = GalacticUnicorn()
gu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_GALACTIC_UNICORN, pen_type=PEN_P8)


DAMPING_FACTOR = 0.95
NUMBER_OF_DROPS = 5
INTENSITY = 10
OFFSET = 0.0  # Try 0.5

# Fill palette with a rainbow sweep
PALETTE_ENTRIES = 255
for x in range(PALETTE_ENTRIES):
    _ = graphics.c