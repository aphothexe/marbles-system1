import gc
import time
import random
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN, PEN_P8
from ulab import numpy

"""
A random, trippy effect.
Experiment with the damping, number of spawns, intensity and offset to change the effect.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

cu = CosmicUnicorn()
cu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_COSMIC_UNICORN, pen_type=PEN_P8)


DAMPING_FACTOR = 0.95
NUMBER_OF_DROPS = 5
INTENSITY = 10
OFFSET = 0.0  # Try 0.5

# Fill palette with a rainbow sweep
PALETTE_ENTRIES = 255
for x in range(PALETTE_ENTRIES):
    _ = graphics.create_pen_hsv(float(x) / PALETTE_ENTRIES + OFFSET, 1.0, 1.0)


def update():
    trippy[:] *= DAMPING_FACTOR

    # Spawn random drops
    for _ in range(NUMBER_OF_DROPS):
        x = random.randint(0, width - 1)
        y = random.randint(0, height - 1)
        trippy[y][x] = random.randint(0, INTENSITY)

    a = numpy.roll(trippy, 1, axis=0)
    b = numpy.roll(trippy, -1, axis=0)
    d = numpy.roll(trippy, 1, axis=1)
    e = numpy.roll(trippy, -1, axis=1)

    # Average over 5 adjacent pixels and