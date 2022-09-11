
import badger2040
import machine
import time

display = badger2040.Badger2040()
display.update_speed(badger2040.UPDATE_TURBO)
display.pen(15)
display.clear()
display.update()

button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_b = machine.Pin(badger2040.BUTTON_B, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_c = machine.Pin(badger2040.BUTTON_C, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_up = machine.Pin(badger2040.BUTTON_UP, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_down = machine.Pin(badger2040.BUTTON_DOWN, machine.Pin.IN, machine.Pin.PULL_DOWN)
# the User button (boot/usr on back of board) is inverted from the others
button_user = machine.Pin(badger2040.BUTTON_USER, machine.Pin.IN, machine.Pin.PULL_UP)


message = None
message_y = 60


def button(pin):
    global message
    if message is not None:
        return
    if pin == button_a:
        message = "Button a"
        return
    if pin == button_b:
        message = "Button b"
        return
    if pin == button_c:
        message = "Button c"
        return
    if pin == button_up:
        message = "Button Up"
        return
    if pin == button_down:
        message = "Button Down"