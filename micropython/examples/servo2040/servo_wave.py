import gc
import time
import math
from pimoroni import Button
from plasma import WS2812
from servo import ServoCluster, servo2040

"""
An example of applying a wave pattern to a group of servos and the LEDs.

Press "Boot" to exit the program.

NOTE: ServoCluster and Plasma WS2812 use the RP2040's PIO system,
and as such may have problems when running code multiple times.
If you encounter issues, try resetting your board.
"""

SPEED = 5             # The speed that the LEDs will cycle at
BRIGHTNESS = 0.4      # The brightness of the LEDs
UPDATES = 50          # How many times to update LEDs and Servos per second
SERVO_EXTENT = 80.0   # How