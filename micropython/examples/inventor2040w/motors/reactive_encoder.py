
import time
from pimoroni import PID, NORMAL_DIR  # , REVERSED_DIR
from inventor import Inventor2040W, MOTOR_A, NUM_LEDS

"""
A demonstration of how a motor with an encoder can be used
as a programmable rotary encoder for user input, with
force-feedback for arbitrary detents and end stops.

Press "User" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motor
