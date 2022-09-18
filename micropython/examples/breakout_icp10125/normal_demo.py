import time

import breakout_icp10125
import pimoroni_i2c

i2c = pimoroni_i2c.PimoroniI2C(4, 5)

icp10125 = breakout_icp1012