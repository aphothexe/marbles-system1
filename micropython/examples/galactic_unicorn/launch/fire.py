import random
from galactic import GalacticUnicorn

graphics = None
palette = None

# setup heat value buffer and fire parameters
width = GalacticUnicorn.WIDTH + 2
height = GalacticUnicorn.HEIGHT + 4
heat = [[0.0 for y in range(height)] for x in range(width)]
fire_spawns = 5
damping_factor = 0.97


def init():
    # a palette of five firey colours (white, yellow, orange, red, smoke)
    global palette
    palette = [
        graphics.create_pen(0, 0, 0),
        graphics.create_pen(20, 20, 20),
        graphics.create_pen(180, 30, 0),
        graphics.create_pen(220, 160, 0),
        graphics.create_pen(255, 255, 180)
    ]


# returns the palette entry for a given heat value
@micropython.native  # noqa: F821
def pen_from_value(value):
    if value < 0.15:
        return palette[0]
    elif value