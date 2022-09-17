import gc
import os
import time
import math
import badger2040w as badger2040
import badger_os
import jpegdec

APP_DIR = "/examples"
FONT_SIZE = 2

changed = False
exited_to_launcher = False
woken_by_button = badger2040.woken_by_button()  # Must be done before we clear_pressed_to_wake

if badger2040.pressed_to_wake(badger2040.BUTTON_A) and badger2040.pressed_to_wake(badger2040.BUTTON_C):
    # Pressing A and C together at start quits app
    exited_to_launcher = badger_os.state_clear_running()
    badger2040.reset_pressed_to_wake()
else:
    # Otherwise restore previously running app
    badger_os.state_launch()


display = badger2040.Badger2040W()
display.set_font("bitmap8")
display.led(128)

jpeg = jpegdec.JPEG(display.display)

state = {
    "page": 0,
    "running": "launcher"
}

badger_os.state_load("launcher", state)

examples = [x[:-3] for x in os.listdir("/examples") if x.endswith(".py")]

# Approximate center lines for buttons A, B and C
centers = (41, 147, 253)

MAX_PAGE = math.ceil(len(examples) / 3)

WIDTH = 296


def map_value(input, in_min, in_max, out_min, out_max):
    return (((input - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min


def draw_disk_usage(x):
    _, f_used, _ = badger_os.get_disk_usage()

    display.set_pen(15)
    display.image(
        bytearray(
            (
                0b00000000,
                0b00111100,
                0b00111100,
                0b00111100,
                0b00111000,
                0b00000000,
                0b00000000,
                0b00000001,
            )
        ),
        8,
        8,
        x,
        4,
    )
    display.rectangle(x + 10, 3, 80, 10)
    display.set_pen(0)
    display.rectangle(x + 11, 4, 78, 8)
    display.set_pen(15)
    display.rectangle(x + 12, 5, int(76 / 100.0 * f_used), 6)
    display.text("{:.2f}%".format(f_used), x + 91, 4, WIDTH, 1.0)


def render():
    display.set_pen(15)
    display.clear()
    display.set_pen(0)

    max_icons = min(3, len(examples[(state["page"] * 3):]))

    for i in range(max_icons):
        x = centers[i]
        label = examples[i + (state["page"] * 3)]
        icon_label = label.replace("_", "-")
        icon = f"{APP_DIR}/icon-{icon_label}.jpg"
        label = label.replace("_", " ")
        jpeg.open_file(icon)
        jpeg.decode(x - 26, 30)
        display.set_pen(0)
        w = display.measure_text(label, FONT_SIZE)
        display.text(label, int(x - (w / 2)), 16 + 80, WIDTH, FONT_SIZE)

    for i in range(MAX_PAGE):
        x = 286
        y = int((128 / 2) - (MAX_PAGE * 10 / 2) + (i * 10))
        display.set_pen(0)
        display.rectangle(x, y, 8, 8)
        if state["page"] != i:
            display.set_pen(15)
            display.rectangle(x + 1, y + 1, 6, 6)

    display.set_pen(0)
    display.rectangle(0, 0, WIDTH, 16)
    draw_disk_usage(90)
    display.set_pen(15)
    display.text("badgerOS", 4, 4, WIDTH, 1.0)

    display.update()


def wait_for_user_to_release_buttons():
    while display.pressed_any():