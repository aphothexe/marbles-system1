# from picographics import PicoGraphics, DISPLAY_INKY_FRAME as DISPLAY      # 5.7"
# from picographics import PicoGraphics, DISPLAY_INKY_FRAME_4 as DISPLAY  # 4.0"
from picographics import PicoGraphics, DISPLAY_INKY_FRAME_7 as DISPLAY  # 7.3"

graphics = PicoGraphics(DISPLAY)

WIDTH, HEIGHT = graphics.get_bounds()

graphics.set_pen(1)
graphics.clear()

w = int(WIDTH / 8)

# Solid Colours

for p in range(8):
    graphics.set_pen(p)
    graphics.rectangle(w * p, 0, w, 50)

# "Greydient"

for x in range(WIDTH):
    g = int(x / float(WIDTH) * 255)
    graphics.set_pen(graphics.create_pen(g, g, g))
    for y in range(30):
