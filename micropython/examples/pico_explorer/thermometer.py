# This example takes the temperature from the Pico's onboard temperature sensor, and displays it on Pico Explorer, along with a little pixelly graph.
# It's based on the thermometer example in the "Getting Started with MicroPython on the Raspberry Pi Pico" book, which is a great read if you're a beginner!

import machine
import time
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER

# set up the hardware
display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)
sensor_temp = machine.ADC(4)

# set up constants for drawing
WIDTH, HEIGHT = display.get_bounds()

BLACK = display.create_pen(0, 0, 0)
WHITE = display.create_pen(255, 255, 255)

conversion_factor = 3.3 / (65535)  # used for calculating a temperature from the raw sensor re