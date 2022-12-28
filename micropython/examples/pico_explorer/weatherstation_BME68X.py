# This example lets you plug a BME680 or BME688 breakout into your Pico Explorer to make a little indoor weather station, with barometer style descriptions.

import time
from breakout_bme68x import BreakoutBME68X
from pimoroni_i2c import PimoroniI2C
from pimoroni import PICO_EXPLORER_I2C_PINS
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER

# set up the hardware
display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)
i2c = PimoroniI2C(**PICO_EXPLORER_I2C_PINS)
bme = BreakoutBME68X(i2c, address=0x76)

# lets set up some pen colours to make drawing easier
TEMPCOLOUR = display.create_pen(0, 0, 0)  # this colour will get changed in a bit
WHITE = display.create_pen(255, 255, 255)
BLACK = display.create_pen(0, 0, 0)
RED = display.create_pen(255, 0, 0)
GREY = display.create_pen(125, 125, 125)


# converts the temperature into a barometer-type description and pen colour
def describe_temperature(temperature):
    global TEMPCOLOUR
    if temperature < 10:
        description = "very cold"
        TEMPCOLOUR = display.create_pen(0, 255, 255)
    elif 10 <= temperature < 20:
        description = "cold"
        TEMPCOLOUR = display.create_pen(0, 0, 255)
    elif 20 <= temperature < 25:
        description = "temperate"
        TEMPCOLOUR = display.create_pen(0, 255, 0)
    elif 25 <= temperature < 30:
        description = "warm"
        TEMPCOLOUR = display.create_pen(255, 255, 0)
    elif te