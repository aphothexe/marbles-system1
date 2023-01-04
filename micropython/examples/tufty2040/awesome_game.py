
import time
import random
from pimoroni import Button
from picographics import PicoGraphics, DISPLAY_TUFTY_2040

display = PicoGraphics(display=DISPLAY_TUFTY_2040)
WIDTH, HEIGHT = display.get_bounds()

# Load the spritsheets so we can flip between them
tilemap = bytearray(128 * 128)
open("s4m_ur4i-pirate-tilemap.rgb332", "rb").readinto(tilemap)

character = bytearray(128 * 128)
open("s4m_ur4i-pirate-characters.rgb332", "rb").readinto(character)

display.set_spritesheet(character)


# Buttons
button_a = Button(7, invert=False)
button_b = Button(8, invert=False)
button_c = Button(9, invert=False)
button_up = Button(22, invert=False)
button_down = Button(6, invert=False)

display.set_backlight(1.0)
display.set_pen(255)
display.clear()


class Player():
    def __init__(self):
        self.reset()

    def reset(self):
        self.x = 150
        self.y = 180
        self.w = 15
        self.h = 30
        self.speed = 10