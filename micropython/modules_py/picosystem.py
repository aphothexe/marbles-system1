import machine
from micropython import const
from breakout_colourlcd240x240 import BreakoutColourLCD240x240
import pimoroni

# Constants and definitions
PICOSYSTEM_LCD_SPI = const(0)
PICOSYSTEM_LCD_RESET = const(4)
PICOSYSTEM_LCD_CS = const(5)
PICOSYSTEM_LCD_SCK = const(6)
PICOSYSTEM_LCD_MOSI = const(7)
PICOSYSTEM