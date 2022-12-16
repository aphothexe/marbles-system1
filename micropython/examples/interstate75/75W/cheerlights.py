# The Interstate75W example updates a pixel every five(ish) minutes
# to display the most recent #cheerlights colour. Discover the most popular
# colours over time, or use it as an avant garde (but colourful) 53 hour clock!
# Find out more about the Cheerlights API at https://cheerlights.com/
#
# To run this example you'll need WIFI_CONFIG.py and network_manager.py from
# the pimoroni-pico micropython/examples/common folder

import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import time
import interstate75
from machine import Timer


URL = 'http://api.thingspeak.com/channels/1417/field/2/last.json'

UPDATE_INTERVAL = 327  # refresh interval in secs. Be nice to free APIs!
# this esoteric number is used so that a column of LEDs equates (approximately) to an hour


def status_handler(mode, status, ip):
    # reports wifi connection status
    print(mode, status, ip)
    print('Connecting to wifi...')
    if status is not None:
        if status:
            print('Wifi connection successful!')
        else:
            print('Wifi connection failed!')


def hex_to_rgb(hex):
    # converts a hex colour code into RGB
    h = hex.lstrip('#')
    r, g, b = (int(h[i:i + 2], 16) for i in (0, 2, 4))
    return r, g, b


def get_data():
    # open the json file
    print(f'