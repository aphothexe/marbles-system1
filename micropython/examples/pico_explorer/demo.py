import time
from picographics import PicoGraphics, DISPLAY_PICO_EXPLORER
from motor import Motor
from pimoroni import Button, Analog, Buzzer

display = PicoGraphics(display=DISPLAY_PICO_EXPLORER)

adc0 = Analog(26)
adc1 = Analog(27)
adc2 = Analog(28)

button_a = Button(12)
button_b = Button(13)
button_x = Button(14)
button_y = Button(15)

BG = display.create_pen(32, 32, 64)
WHITE = display.create_pen(255, 255, 255)

ADC0_PEN = display.create_pen(255, 0, 0)
ADC1_PEN = display.create_pen(0, 255, 0)
ADC2_PEN = display.create_pen(0, 0, 255)

MOTOR1 = Motor((8, 9))
MOTOR2 = Motor((10, 11))

BUZZER = Buzzer(0)

i = 1

while True:
    display.set_pen(BG)
    display.clear()

    # Read the ADCs
    adc0v = int(adc0.read_voltage() / 3.3 * 120)
    adc1v = int(adc1.read_voltage() / 3.3 * 120)
    adc2v = i