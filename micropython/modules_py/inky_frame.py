from pimoroni import ShiftRegister, PWMLED
from machine import Pin, I2C, RTC
from wakeup import get_shift_state, reset_shift_state
from micropython import const
import pcf85063a
import ntptime
import time

BLACK = const(0)
WHITE = const(1)

GREEN = const(2)
BLUE = const(3)
RED = const(4)
YELLOW = const(5)
ORANGE = const(6)
TAUPE = const(7)

SR_CLOCK = const(8)
SR_LATCH = const(9)
SR_OUT = const(10)

LED_A = const(11)
LED_B = const(12)
LED_C = const(13)
LED_D = const(14)
LED_E = const(15)

LED_BUSY = const(6)
LED_WIFI = const(7)

HOLD_VSYS_EN = const(2)

RTC_ALARM = const(2)
EXTERNAL_TRIGGER = const(1)
EINK_BUSY = const(0)

SHIFT_STATE = get_shift_state()

reset_shift_state()

i2c = I2C(0)
rtc = pcf85063