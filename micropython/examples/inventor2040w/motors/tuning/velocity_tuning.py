import time
from pimoroni import PID, NORMAL_DIR  # , REVERSED_DIR
from inventor import Inventor2040W, MOTOR_A

"""
A program to aid in the discovery and tuning of motor PID
values for velocity control. It does this by commanding the
motor to drive repeatedly between two setpoint speeds and
plots the measured response.

Press "User" to exit the program.
"""

GEAR_RATIO = 50                         # The gear ratio of the motor

DIRECTION = NORMAL_DIR                  # The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor'