import badger2040
import time
import gc
import badger_os

# **** Put the name of your text file here *****
text_file = "book.txt"  # File must be on the MicroPython device


try:
    open(text_file, "r")
except OSError:
    try:
        # If the specified file doesn't exist,
        # pre-populate with Wind In The Willows
        import witw
        with open(text_file, "wb") as f:
            f.write(witw.data())
            f.flush()
            time.sleep(0.1)
        del witw
    except ImportError:
        pass


gc.collect()

# Global Constants
WIDTH = badger2040.WIDTH
HEIGHT = badger2040.HEIGHT

ARROW_THICKNESS = 3
ARROW_WIDTH = 18
ARROW_HEIGHT = 14
ARROW_PADDING = 2

TEXT_PADDING = 4
TEXT_WIDTH = WIDTH - TEXT_PADDING - TEXT_PADDING - ARROW_WIDTH

FONTS = ["sans", "gothic", "cursive", "serif"]
FONT_THICKNESSES = [2, 1, 1, 2]
# ------------------------------
#      Drawing functions
# ------------------------------


# Draw a upward arrow
def draw_up(x, y, width, height, thickness, padding):
    border = (thickness // 4) + padding
    display.line(x + border, y + height - border,
 