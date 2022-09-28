import time
from cosmic import CosmicUnicorn
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

'''
A collection of copies of classic terminal styles including
C64, MS-DOS, Spectrum, and more. Images and text are drawn
pixel by pixel from a pattern of Os and Xs.
You can adjust the brightness with LUX + and -.
'''

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

prompt_x = 0
prompt_y = 4

c64 = [
    "                                ",
    "                                ",
    " OOOOO   OOOOOO    OO    OOOO   ",
    " OO  OO  OO       OOOO   OO OO  ",
    " OO  OO  OO      OO  OO  OO  OO ",
    " OOOOO   OOOO    OOOOOO  OO  OO ",
    " OOOO    OO      OO  OO  OO  OO ",
    " OO OO   OO      OO  OO  OO OO  ",
    " OO  OO  OOOOOO  OO  OO  OOOO   ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    " OO  OO      XXXXXXX            ",
    " OO  OO      XXXXXXX            ",
    " OO  OO      XXXXXXX            ",
    "  OOOO       XXXXXXX            ",
    "   OO        XXXXXXX            ",
    "   OO    OO  XXXXXXX            ",
    "   OO    OO  XXXXXXX            ",
    "             XXXXXXX            ",
    "                                "
]
FOREGROUND_C64 = (230, 210, 250)
BACKGROUND_C64 = (20, 20, 120)

spectrum = [
    "                                ",
    "                                ",
    " O        OOOO    OOOO   OOOOO  ",
    " O       O    O  O    O  O    O ",
    " O       O    O  O    O  O    O ",
    " O       O    O  OOOOOO  O    O ",
    " O       O    O  O    O  O    O ",
    " OOOOOO   OOOO   O    O  OOO