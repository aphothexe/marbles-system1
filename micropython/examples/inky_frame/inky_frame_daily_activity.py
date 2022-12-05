import gc
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import ujson
from urllib import urequest
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"
from machine import Pin
from pimoroni_i2c import PimoroniI2C
from pcf85063a import PCF85063A
import 