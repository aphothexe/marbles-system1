import gc
import random
from urllib import urequest
from ujson import load
import qrcode

gc.collect()  # We're really gonna need that RAM!

graphics = None

WIDTH = 0
HEIGHT = 0

FILENAME = "random-joke.jpg"

JOKE_IDS = "https://pimoroni.github.io/feed2image/jokeapi-ids.txt"
JOKE_IMG = "https://pimoroni.github.io/feed2image/jokeapi-{}.json"

UPDATE_INTERVAL = 60

gc.collect()  # Claw back some RAM!

# We don't have the RAM to store the list of Joke IDs in memory.
# the first line of `jokeapi-ids.txt` is a COUNT of IDs.
# Grab it, then pick a random line between 0 and COUNT.
# Seek to that line and ...y'know... there's our totally random joke ID

joke = []


def measure_qr_code(size, code):
    w, h = code.get_size()
    module_size = int(size / w)
    return module_size * w, module_size


def draw_qr_code(ox, oy, size, code):
    size, module_size = measure_qr_code(size, code)
    graphics.set_pen(1)
    graphics.rectangle(ox, oy, size, size)
    graphics.set_pen(0)
    for x in range(size):
        for y in range(size):
            if code.get_module(x, y):
                graphics.rectangle(ox + x * module_size, oy + y * module_size, module_size, module_size)


def update():
    global joke

    try:
        socket = urequest.urlopen(JOKE_IDS)
    except OSError:
        return

    # Get the first line, which is a count of the joke IDs
    number_of_lines = int(socket.readline().decode("ascii"))
    print("Total jokes {}".format(number_of_lines))

    # Pick a random joke (by its line number)
    line = random.randint(0, number_of_lines)
  