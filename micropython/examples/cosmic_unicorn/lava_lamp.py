import time
import random
import math
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

'''
A 70s-tastic, procedural rainbow lava lamp.

You can adjust the brightness with LUX + and -.
'''

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

blob_count = 10


class Blob():
    def __init__(self):
        self.x = float(random.randint(0, width - 1))
        self.y = float(random.randint(0, height - 1))
        self.r = (float(random.randint(0, 40)) / 10.0) + 5.0
        self.dx = (float(random.randint(0, 2)) / 10.0) - 0.1
        self.dy = (float(random.randint(0, 2)) / 10.0) - 0.05  # positive bias


@micropython.native  # noqa: F821
def setup_portrait():
    global width, height, liquid, blobs
    width = CosmicUnicorn.HEIGHT
    height = CosmicUnicorn.WIDTH
    liquid = [[0.0 for y in range(height)] for x in range(width)]
    blobs = [Blob() for i in range(blob_count)]


hue = 0.0


@micropython.native  # noqa: F821
def from_hsv(h, s, v):
    i = math.floor(h * 6.0)
    f = h * 6.0 - i
    v *= 255.0
    p = v * (1.0 - s)
    q = v * (1.0 - f * s)
    t = v * (1.0 - (1.0 - f) * s)

    i = int(i) % 6
    if i == 0:
        return graphics.create_pen(int(v), int(t), int(p))
    if i == 1:
        return graphics.create_pen(int(q), int(v), int(p))
    if i == 2:
        return graphics.create_pen(int(p), int(v), int(t))
    if i == 3:
        return graphics.create_pen(int(p), int(q), int(v))
    if i == 4:
        return graphics.create_pen(int(t), int(p), int(v))
    if i == 5:
        return graphics.create_pen(int(v), int(p), int(q))


@micropython.native  # noqa: F821
def update_liquid():
    for y in range(height):
        for x in range(width):
            liquid[x][y] = 0.0
