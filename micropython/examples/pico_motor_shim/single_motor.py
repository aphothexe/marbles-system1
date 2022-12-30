import time
import math
from motor import Motor, pico_motor_shim

"""
Demonstrates how to create a Motor object and control it.
"""

# Create a motor
m = Motor(pico_motor_shim.MOTOR_1)

# Enable the motor
m.enable()
time.sleep(2)

# Drive at full positive
m.full_positive()
time.sleep(2)

# Stop moving
m.stop()
time.sleep(2)

# Dr