
import gc
import time
import math
import badger2040
from badger2040 import WIDTH
import launchericons
import badger_os

# Reduce clock speed to 48MHz
badger2040.system_speed(badger2040.SYSTEM_NORMAL)

changed = False
exited_to_launcher = False
woken_by_button = badger2040.woken_by_button()  # Must be done before we clear_pressed_to_wake

if badger2040.pressed_to_wake(badger2040.BUTTON_A) and badger2040.pressed_to_wake(badger2040.BUTTON_C):
    # Pressing A and C together at start quits app
    exited_to_launcher = badger_os.state_clear_running()