import time
import math
import random
from inventor import Inventor2040W, SERVO_1

"""
An example of how to move a servo smoothly between random positions.

Press "User" to exit the program.
"""

UPDATES = 50            # How many times to update Servos per second
TIME_FOR_EACH_MOVE = 2  # The time to travel between each random value
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES

SERVO_EXTENT = 80       # How far from zero to move the servo
USE_COSINE = True       # Whether or not to use a cosine path between values

# Create a new Inventor2040W and get a servo from it
board = Inventor2040W()
s = board.servos[SERVO_1]

# Get the initial value and create