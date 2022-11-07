# This example shows you a simple, non-interrupt way of reading GFX Pack's buttons with a loop that checks to see if buttons are pressed.

import time
from gfx_pack import GfxPack, SWITCH_A, SWITCH_B, SWITCH_C, SWITCH_D, SWITCH_E

gp = GfxPack()
display = gp.display

WIDTH, HEIGHT = display.get_bounds()
display.set_backlight(0)  # turn off the white component of the backlight


# sets up a handy function we can call to clear the screen
def clear():
    display.set_pen(0)
    display.clear()
    display.set_pen(15)


# set up
display.set_font("