import gc
import time
import random
from cosmic import CosmicUnicorn, Channel
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN, PEN_P8
from ulab import numpy

"""
A random, computer effect.
Experiment with the damping, number of spawns and intensity to change the effect.
"""

# MAXIMUM OVERKILL
# machine.freq(250_000_000)

DAMPING_FACTOR = 0.95
NUMBER_OF_LIGHTS = 10
INTENSITY = 20

volume = 0.5

cu = CosmicUnicorn()
cu.set_brightness(0.5)
graphics = PicoGraphics(DISPLAY_COSMIC_UNICORN, pen_type=PEN_P8)

boopety_beepety = cu.synth_channel(0)
boopety_beepety.configure(
    waveforms=Channel.SQUARE | Channel.SINE,
    attack=0.1,
    decay=0.1,
    sustain=0.0,
    release=0.5,
    volume=volume
)

cu.play_synth()

# Fill palette with a yellow
r, g, b = (230, 150, 0)
PALETTE_ENTRIES = 255
for x in range(PALETTE_ENTRIES):
    _ = graphics.create_pen(r * x // PALETTE_ENTRIES, g * x // PALETTE_ENTRIES, b)


def update():
    computer[:] *= DAMPING_FACTOR

    # Spawn random drops
    for _ in range(NUMBER_OF_LIGHTS):
        x = random.randint(0, width - 1)
        y = random.randint(0, height - 1)