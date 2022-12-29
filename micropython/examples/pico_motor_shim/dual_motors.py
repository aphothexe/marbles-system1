import time
import math
from motor import Motor, pico_motor_shim
# from pimoroni import REVERSED_DIR

"""
Demonstrates how to create two Motor objects and control them together.
"""

# Create a list of motors
MOTOR_PINS = [pico_motor_shim.MOTOR_1, pico_motor_shim.MOTOR_2]
motors = [Motor(pins) for pins in MOTOR_PINS]

# Uncomment the below lines (and the top import) to
#