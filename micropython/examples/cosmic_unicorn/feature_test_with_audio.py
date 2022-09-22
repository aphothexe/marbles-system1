import gc
import time
import math
from machine import Timer
from cosmic import CosmicUnicorn, Channel
from picographics import PicoGraphics, DISPLAY_COSMIC_UNICORN as DISPLAY

'''
Displays some text, gradients and colours and demonstrates button use.
Also demonstrates some of the audio / synth features.

- Button A plays a synth tune
- Button B plays a solo channel of the synth tune
- Button C plays a sinewave (it's frequency can be adjusted with VOL + and -)
- Button D plays a second sinewave (it's frequency can be adjusted with LUX + and -)
- Sleep button stops the sounds
'''

gc.collect()

cu = CosmicUnicorn()
graphics = PicoGraphics(DISPLAY)

width = CosmicUnicorn.WIDTH
height = CosmicUnicorn.HEIGHT

SONG_LENGTH = 384
HAT = 20000
BASS = 500
SNARE = 6000
SUB = 50

melody_notes = (
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 220, 0, 196, 0, 147, 0, 175, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 220, 0, 196, 0, 147, 0, 175, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 262, 0, 294, 0, 392, 0, 440, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

rhythm_notes = (
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0)

drum_beats = (
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0,
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0,
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0)

hi_hat = (
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1,
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1,
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1)

bass_notes = (
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0,
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0)

notes = [melody_notes, rhythm_notes, drum_beats, hi_hat, bass_notes]
channels = [cu.synth_channel(i) for i in range(len(notes) + 1)]  # Extra channel for tones

# Configure the synth to play our notes
channels[0].configure(waveforms=Channel.TRIANGLE + Channel.SQUARE,
                      attack=0.016,
                      decay=0.168,
                      sustain=0xafff / 65535,
                      release=0.168,
                      volume=10000 / 65535)

channels[1].configure(waveforms=Channel.SINE + Channel.SQUARE,
                      attack=0.038,
                      decay=0.300,
                      sustain=0,
                      release=0,
                      volume=12000 / 65535)

channels[2].configure(waveforms=Channel.NOISE,
                      attack=0.005,
                      decay=0.010,
                      sustain=16000 / 65535,
                      release=0.100,
                      volume=18000 / 65535)

channels[3].configure(waveforms=Channel.NOISE,
                      attack=0.005,
                      decay=0.005,
                      sustain=8000 / 65535,
                      release=0.040,
                      volume=8000 / 65535)

channels[4].configure(waveforms=Channel.SQUARE,
                      attack=0.010,
                      decay=0.100,
                      sustain=0,
                      release=0.500,
                      volume=12000 / 65535)


def gradient(r, g, b):
    for y in range(0, height):
        for x in range(0, width):
            graphics.set_pen(graphics.create_pen(int((r * x) / 52), int((g * x) / 52), int((b * x) / 52)))
            graphics.pixel(x, y)


def grid(r, g, b):
    for y in range(0, height):
        for x in range(0, width):
            if (x + y) % 2 == 0:
                graphics.set_pen(graphics.create_pen(r, g, b))
            else:
                graphics.set_pen(0)
            graphics.pixel(x, y)


def outline_text(text):
    ms = time.ticks_ms()

    graphics.set_font("bitmap8")
    v = int((math.sin(ms / 100.0) + 1.0) * 127.0)
    w = graphics.measure_text(text, 1)

    x = int(32 / 2 - w / 2 + 1)
    y = 12

    graphics.set_pen(0)
    graphics.text(text, x - 1, y - 1, -1, 1)
    graphics.text(text, x, y - 1, -1, 1)
    graphics.text(text, x + 1, y - 1, -1, 1)
    graphics.text(text, x - 1, y, -1, 1)
    graphics.text(text, x + 1, y, -1, 1)
    graphics.text(text, x - 1, y + 1, -1, 1)
    graphics.text(text, x, y + 1, -1, 1)
    graphics.text(text, x + 1, y + 1, -1, 1)

    graphics.set_pen(graphics.create_pen(v, v, v))
    graphics.text(text, x, y, -1, 1)


cu.set_brightness(0.5)

# Vars for storing button state
was_a_pressed = False
was_b_pressed = False
was_c_pressed = False
was_d_pressed = False
was_z_pressed = False

# The two frequencies to play
tone_a = 0
tone_b = 0

# The current synth beat
beat = 0

text = ""


def next_beat():
    global beat
    for i in range(5):
        if notes[i][beat] > 0:
            channels[i].frequency(notes[i][beat])
            channels[i].trigger_attack()
        elif notes[i][beat] == -1:
            channels[i].trigger_release()

    beat = (beat + 1) % SONG_LENGTH


def tick(timer):
    next_beat()


timer = Timer(-1)

synthing = False


while True:

    time_ms = time.ticks_ms()
    test = (time_ms // 1000) % 5

    if cu.is_pressed(CosmicUnicorn.SWITCH_A):
        if not was_a_pressed:
            channels[0].volume(10000 / 65535)
            channels[1].volume(12000 / 65535)
            channels[2].volume(18000 / 65535)
            channels[3].volume(8000 / 65535)
            channels[4].volume(12000 