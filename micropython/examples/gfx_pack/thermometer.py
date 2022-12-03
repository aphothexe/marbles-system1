import time
import machine
from gfx_pack import GfxPack
from breakout_bme68x import BreakoutBME68X, STATUS_HEATER_STABLE

"""
GFX temp DEMO

This demo uses a BME680 or BME688 attached to the QWST connector to
measure temperature, pressure, and humidity and display it on the GFX display,
or the internal temperature sensor can be used in place of the
BME68x breakout. Just change use_bme68x_bre