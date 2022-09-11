import math
import time
from random import random

import machine

import badger2040

# Overclock the RP2040 to run the sim faster
badger2040.system_speed(badger2040.SYSTEM_TURBO)

# ------------------------------
#        Program setup
# ------------------------------

# Global constants
CELL_SIZE = 6  # Size of cell in pixels
INITIAL_DENSITY = 0.3  # Density of cells at start

# Create a new Badger and set it to update TURBO
screen = badger2040.Badger2040()
screen.led(128)
screen.update_speed(badger2040.UPDATE_TURBO)

restart = False  # should sim be restarted

# ------------------------------
#     Button functions
# ------------------------------


# Button handling function
def button(pin):
    global restart
    # if 'a' button is pressed, restart the sim
    if pin == button_a:
        restart = True


# Set up button
button_a = machine.Pin(badger2040.BUTTON_A, machine.Pin.IN, machine.Pin.PULL_DOWN)
button_a.irq(trigger=machine.Pin.IRQ_RISING, handler=button)

# ------------------------------
#      Screen functions
# ------------------------------


# Remove everything from the screen
def init_screen():
    screen.update_speed(badger2040.UPDATE_NORMAL)
    screen.pen(15)
    screen.clear()
    screen.update()
    screen.update_speed(badger2040.UPDATE_TURBO)


# ------------------------------
#       Classes
# ------------------------------

# Define a 'cell'
class Cell:
    def __init__(self):
        self._alive = False

    def make_alive(self):
        self._alive = True

    def make_dead(self):
        self._alive = False

    def is_alive(self):
        return self._alive


# Define the whole board
class Board:
    def __init__(self):
        self._rows = math.floor(badger2040.WIDTH / CELL_SIZE)
        self._columns = math.floor(badger2040.HEIGHT / CELL_SIZE)
        self._grid = [[Cell() for _ in range(self._columns)] for _ in range(self._rows)]

        self._initialise_board()

    # Draw the board to the screen
    def draw_board(self):
        row_idx = 0
        column_idx = 0

        for row in self._grid:
            column_idx = 0
            for cell in row:
                if cell.is_alive():
                    screen.pen(0)
                else:
                    screen.pen(15)
                screen.rectangle(
                    row_idx * CELL_SIZE, column_idx * CELL_SIZE, CELL_SIZE, CELL_SIZE
                )
                column_idx += 1
            row_idx += 1

        screen.update()

    # Generate the first iteration of the board
    def _initialise_board(self):
        for row in self._grid:
            for cell in row:
                if random() <= INITIAL_DENSITY:
                    cell.make_alive()

    # Get the neighbour cells for a given cell
    def get_neighbours(self, current_row, current_column):
        # Cells either side of current cell
        neighbour_min = -1
        neighbour_max = 2
        neighbours = []

        for row in range(neighbour_min, neighbour_max):
            for column in range(neighbour_min, neighbour_max):
                neighbour_row = current_row + row
                neighbour_column = current_column + column
                # Don't count the current cell
                if not (
            