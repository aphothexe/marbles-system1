import time

graphics = None
palette = None


c64 = [
    "                                                     ",
    "                                                     ",
    " OOOOO   OOOOOO    OO    OOOO    OO  OO      XXXXXXX ",
    " OO  OO  OO       OOOO   OO OO   OO  OO      XXXXXXX ",
    " OO  OO  OO      OO  OO  OO  OO  OO  OO      XXXXXXX ",
    " OOOOO   OOOO    OOOOOO  OO  OO   OOOO       XXXXXXX ",
    " OOOO    OO      OO  OO  OO  OO    OO        XXXXXXX ",
    " OO OO   OO      OO  OO  OO OO     OO    OO  XXXXXXX ",
    " OO  OO  OOOOOO  OO  OO  OOOO      OO    OO  XXXXXXX ",
    "                                             XXXXXXX ",
    "                                                     "
]
FOREGROUND_C64 = (230, 210, 250)
BACKGROUND_C64 = (20, 20, 120)

spectrum = [
    "                                                     ",
    "                                                     ",
    " O        OOOO    OOOO   OOOOO     O O  O O XXXXXXXX ",
    " O       O    O  O    O  O    O    O O  O O X XXXXXX ",
    " O       O    O  O    O  O    O             X XXXXXX ",
    " O       O    O  OOOOOO  O    O             X XXXXXX ",
    " O       O    O  O    O  O    O             X XXXXXX ",
    " OOOOOO   OOOO   O    O  OOOOO              X XXXXXX ",
    "                                            X      X ",
    "                                            XXXXXXXX ",
    "                                                     "
]
FOREGROUND_SPECTRUM = (0, 0, 0)
BACKGROUND_SPECTRUM = (180, 150, 150)

bbc_micro = [
    "                                                     ",
    "                                                     ",
    " OOOOO    OO    OOOO   OOO    OOOO      O            ",
    " O    O  O  O  O    O   O    O    O      O           ",
    " O    O O    O O        O    O            O          ",
    " OOOOO  O    O  OOOO    O    O             O         ",
    " O    O OOOOOO      O   O    O            O          ",
    " O    O O    O O    O   O    O    O      O           ",
    " OOOOO  O    O  OOOO   OOO    OOOO      O            ",
    "                                             XXXXXXX ",
    "                                                     "
]
FOREGROUND_BBC_MICRO = (255, 255, 255)
BACKGROUND_BBC_MICRO = (0, 0, 0)

PROMPT_C64 = 0
PROMPT_SPECTRUM = 1