import badger2040w
from badger2040w import WIDTH

TEXT_SIZE = 1
LINE_HEIGHT = 15

display = badger2040w.Badger2040W()
display.led(128)

# Clear to white
display.set_pen(15)
display.clear()

display.set_font("bitmap8")
display.set_pen(0)
display.rectangle(0, 0, WIDTH, 16)
display.set_pen(15)
display.text("badgerOS", 3, 4, WIDTH, 1)
display.text("info", WIDTH - display.measure_text("help", 0.4) - 4, 4, WIDTH, 1)

display.set_pen(0)

y = 16 + int(LINE_HEIGHT / 2)

display.text("Made by Pimoroni, powered by MicroPython", 5, y, WIDTH, TEXT_SIZE)
y += LINE_HEIGHT
display.text("Dual-core RP2040, 133MHz, 264KB RAM", 5, y, WIDTH, TEXT_SIZE)
y