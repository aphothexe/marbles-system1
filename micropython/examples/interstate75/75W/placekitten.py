import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
from urllib import urequest
from interstate75 import Interstate75, DISPLAY_INTERSTATE75_128X64
import jpegdec
import random

"""
Grab a random image from PlaceKitten.com
and display it on Interstate 75 W.

To run this example you'll need WIFI_CONFIG.py and network_manager.py from
the pimoroni-pico micropython/examples/common folder.
"""

i75 = Interstate75(display=DISPLAY_INTERSTATE75_128X64)
graphics = i75.display

WIDTH = i75.width
HEIGHT = i75.height
FILENAME = "placekitten.jpg"
ENDPOINT = "http://placekitten.com/{0}/{1}"

# some colours to draw with
WHITE = graphics.create_pen(255, 255, 255)
BLACK = graphics.create_pen(0, 0, 0)


def status_handler(mode, status, ip):
    graphics.set_font("bitmap8")
    graphics.set_pen(BLACK)
    graphics.clear()
    graphics.set_pen(WHITE)
    graph