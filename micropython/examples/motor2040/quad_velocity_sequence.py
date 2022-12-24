import gc
import time
from motor import Motor, motor2040
from encoder import Encoder, MMME_CPR
from pimoroni import Button, PID, REVERSED_DIR

"""
A demonstration of driving all four of Motor 2040's motor outputs through a
sequence of velocities, with the help of their attached encoders and PID control.

Press "Boot" to exit the program.
"""

# Wheel friendly names
FL = 2
FR = 3
RL = 1
RR = 0

GEAR_RATIO = 50                         # The gear ratio of the motors
COUNTS_PER_REV = MMME_CPR * GEAR_RATIO  # The counts per revolution of each motor's output shaft

SPEED_SCALE = 5.4                       # The scaling to apply to each motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
TIME_FOR_EACH_MOVE = 2                  # The time to travel between each value
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
PRINT_DIVIDER = 4                       # How many of the upda