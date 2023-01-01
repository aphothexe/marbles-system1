import time
import plasma
from plasma import plasma_stick
from machine import RTC

"""
Spooky moon simulator!
The LEDs will get brighter as midnight approaches!
It won't do much in the day...
Needs to be run from Thonny to get the right time.
"""

# Set how many LEDs you have
NUM_LEDS = 50

# pick a hue (0 - 360° on the colour wheel)
# warm white moon - 60, blue moon - 230 , blood moon - 0
HUE = 60
SATURATION = 0.2  # increase this for a more colourful moon (max 1.0)

# when to start counting down from, in seconds before midnight
# eg from 10pm = 2 hours = 2 * 60 * 60 = 7200
COUNT_FROM = 14400

# set up the WS2812 / NeoPixel™ LEDs
le