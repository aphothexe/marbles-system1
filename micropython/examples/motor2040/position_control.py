
import gc
import time
import math
import random
from motor import Motor, motor2040
from encoder import Encoder, MMME_CPR
from pimoroni import Button, PID, NORMAL_DIR  # , REVERSED_DIR

"""
An example of how to move a motor smoothly between random positions,
with the help of it's attached encoder and PID control.

Press "Boot" to exit the program.
"""

MOTOR_PINS = motor2040.MOTOR_A          # The pins of the motor being profiled
ENCODER_PINS = motor2040.ENCODER_A      # The pins of the encoder attached to the profiled motor
GEAR_RATIO = 50                         # The gear ratio of the motor
COUNTS_PER_REV = MMME_CPR * GEAR_RATIO  # The counts per revolution of the motor's output shaft

DIRECTION = NORMAL_DIR                  # The direction to spin the motor in. NORMAL_DIR (0), REVERSED_DIR (1)
SPEED_SCALE = 5.4                       # The scaling to apply to the motor's speed to match its real-world speed

UPDATES = 100                           # How many times to update the motor per second
UPDATE_RATE = 1 / UPDATES
TIME_FOR_EACH_MOVE = 1                  # The time to travel between each random value
UPDATES_PER_MOVE = TIME_FOR_EACH_MOVE * UPDATES
PRINT_DIVIDER = 4                       # How many of the updates should be printed (i.e. 2 would be every other update)

# Multipliers for the different printed values, so they appear nicely on the Thonny plotter
SPD_PRINT_SCALE = 20                    # Driving Speed multipler

POSITION_EXTENT = 180                   # How far from zero to move the motor, in degrees
INTERP_MODE = 2                         # The interpolating mode between setpoints. STEP (0), LINEAR (1), COSINE (2)

# PID values
POS_KP = 0.14                           # Position proportional (P) gain
POS_KI = 0.0                            # Position integral (I) gain
POS_KD = 0.0022                         # Position derivative (D) gain


# Free up hardware resources ahead of creating a new Encoder
gc.collect()

# Create a motor and set its speed scale
m = Motor(MOTOR_PINS, direction=DIRECTION, speed_scale=SPEED_SCALE)

# Create an encoder, using PIO 0 and State Machine 0
enc = Encoder(0, 0, ENCODER_PINS, direction=DIRECTION, counts_per_rev=COUNTS_PER_REV, count_microsteps=True)

# Create the user button
user_sw = Button(motor2040.USER_SW)

# Create PID object for position control
pos_pid = PID(POS_KP, POS_KI, POS_KD, UPDATE_RATE)

# Enable the motor to get started
m.enable()


update = 0
print_count = 0

# Set the initial value and create a random end value between the extents
start_value = 0.0
end_value = random.uniform(-POSITION_EXTENT, POSITION_EXTENT)

# Continually move the motor until the user button is pressed
while not user_sw.raw():

    # Capture the state of the encoder
    capture = enc.capture()

    # Calculate how far along this movement to be
    percent_along = min(update / UPDATES_PER_MOVE, 1.0)

    if INTERP_MODE == 0:
        # Move the motor instantly to the end value
        pos_pid.setpoint = end_value
    elif INTERP_MODE == 2:
        # Move the motor between values using cosine
        pos_pid.setpoint = (((-math.cos(percent_along * math.pi) + 1.0) / 2.0) * (end_value - start_value)) + start_value
    else:
        # Move the motor linearly between values
        pos_pid.setpoint = (percent_along * (end_value - start_value)) + start_value

    # Calculate the velocity to move the motor closer to the position setpoint
    vel = pos_pid.calculate(capture.degrees, capture.degrees_per_second)

    # Set the new motor driving speed
    m.speed(vel)

    # Print out the current motor values and their setpoints, but only on every multiple
    if print_count == 0:
        print("Pos =", capture.degrees, end=", ")
        print("Pos SP =", pos_pid.setpoint, end=", ")
        print("Speed = ", m.speed() * SPD_PRINT_SCALE)

    # Increment the print count, and wrap it
    print_count = (print_count + 1) % PRINT_DIVIDER

    update += 1     # Move along in time

    # Have we reached the end of this movement?
    if update >= UPDATES_PER_MOVE:
        update = 0  # Reset the counter

        # Set the start as the last end and create a new random end value
        start_value = end_value
        end_value = random.uniform(-POSITION_EXTENT, POSITION_EXTENT)

    time.sleep(UPDATE_RATE)

# Disable the servo
m.disable()