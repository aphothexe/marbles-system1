import time
import math
import random
from pimoroni import PID, NORMAL_DIR  # , REVERSED_DIR
from inventor import Inventor2040W, MOTOR_A

"""
An example of how to drive a motor smoothly between random speeds,
with the help of it's attached encoder and PID control.

Press "User" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motor

DIRECTION = NORMAL_DIR                  # The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
TIME_FOR_EACH_MOVE = 1                  # The time to travel between each random value, in seconds
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

# Multipliers for the different printed values, so they appear nicely on the Thonny plotter
ACC_PRINT_SCAL