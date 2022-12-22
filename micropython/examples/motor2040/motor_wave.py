import gc
import time
import math
from pimoroni import Button
from plasma import WS2812
from motor import Motor, motor2040

"""
An example of applying a wave pattern to a group of motors and the LED.

Press "Boot" to exit the program.

NOTE: Plasma WS2812 uses the RP2040's PIO system, and as
such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

SPEED = 5             # The speed that the LEDs and motors will cycle at
BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and motors per second
SPEED_EXTENT = 1.0    # How far from zero to drive the motors

# Free up hardware resources ahead of creating a new MotorCluster
gc.collect()

# Create a list of motors
MOTOR_PINS = [motor2040.MOTOR_A, motor2040.MOTOR