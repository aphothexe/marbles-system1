# Motors and Motor 2040 <!-- omit in toc -->

The Motor library lets you drive DC motors from a Raspberry Pi Pico or any other RP2040-based board via connected h-bridge drivers.

An easy way to add an h-bridge driver to the Pico is to attach a [Pimoroni Pico Motor Shim](https://pimoroni.com/picomotorshim).
Alternatively, an RP2040-based board with integrated drivers could be used, such as the [Pimoroni Motor 2040](https://pimoroni.com/motor2040) (coming soon).

This library offers two motor implementations:
* a `Motor` class that uses hardware PWM to drive a single motor, with support for up to 8 motors.
* a `MotorCluster` class that uses Programmable IO (PIO) hardware to drive up to 15 motors.


## Table of Content
- [Pico Motor Shim](#pico-motor-shim)
  - [Reading the User Button](#reading-the-user-button)
  - [Pin Constants](#pin-constants)
    - [Motor Pins](#motor-pins)
    - [I2C Pins](#i2c-pins)
    - [Button Pin](#button-pin)
  - [Other Constants](#other-constants)
- [Motor 2040](#motor-2040)
  - [Reading the User Button](#reading-the-user-button-1)
  - [Reading the Sensors](#reading-the-sensors)
    - [Configuring Pulls](#configuring-pulls)
    - [Fault Sensing](#fault-sensing)
  - [Reading the Encoders](#reading-the-encoders)
  - [Controlling the LED](#controlling-the-led)
  - [Pin Constants](#pin-constants-1)
    - [Motor Pins](#motor-pins-1)
    - [Encoder Pins](#encoder-pins)
    - [LED Pin](#led-pin)
    - [I2C Pins](#i2c-pins-1)
    - [Button Pin](#button-pin-1)
    - [Address Pins](#address-pins)
    - [ADC Pins](#adc-pins)
  - [Other Constants](#other-constants-1)
    - [Counts](#counts)
    - [Addresses](#addresses)
    - [Sensing](#sensing)
- [Motor](#motor)
  - [Getting Started](#getting-started)
  - [Control by Speed](#control-by-speed)
    - [Full Speed](#full-speed)
    - [Stopping](#stopping)
    - [Calibration](#calibration)
  - [Control by Percent](#control-by-percent)
  - [Control by Duty Cycle](#control-by-duty-cycle)
    - [Duty Deadzone](#duty-deadzone)
  - [Frequency Control](#frequency-control)
  - [Configuration](#configuration)
    - [Direction](#direction)
    - [Decay Mode](#decay-mode)
    - [Driver Type](#driver-type)
  - [Function Reference](#function-reference)
  - [Constants Reference](#constants-reference)
  - [PWM Limitations](#pwm-limitations)
- [MotorCluster](#motorcluster)
  - [Getting Started](#getting-started-1)
  - [Control by Speed](#control-by-speed-1)
    - [Full Speed](#full-speed-1)
    - [Stopping](#stopping-1)
    - [Calibration](#calibration-1)
  - [Control by Percent](#control-by-percent-1)
  - [Control by Duty Cycle](#control-by-duty-cycle-1)
    - [Duty Deadzone](#duty-deadzone-1)
  - [Frequency Control](#frequency-control-1)
  - [Phase Control](#phase-control)
  - [Configuration](#configuration-1)
    - [Direction](#direction-1)
    - [Decay Mode](#decay-mode-1)
  - [Delayed Loading](#delayed-loading)
  - [Function Reference](#function-reference)
  - [Constants Reference](#constants-reference)
  - [PIO Limitations](#pio-limitations)


## Pico Motor Shim

Pico Motor Shim (or Motor Shim for Pico, if you prefer) is a small board featuring a DRV8833 dual h-bridge motor driver IC, a user button, and a Qw/st connector. It attaches to the underside of a Raspberry Pi Pico towards the USB end, and connects to two motors with Motor Connector Shims (MCS) attached via 2 pin JST-ZH cables.

For more information on this board and its accessories, check out the store page: [pimoroni.com/picomotorshim](https://pimoroni.com/picomotorshim).


### Reading the User Button

The Pico Motor Shim has a handy button onboard, offering a tactile way to interact with your program. To simplify the use of this and other buttons, the `pimoroni` module contains a `Button` class that handles button debounce and auto-repeat.

```python
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

To set up the user button, first import the `Button` class from the `pimoroni` module and the pin constant for the button from `motor`:
```python
from pimoroni import Button
from motor import picomotorshim
```

Then create an instance of `Button` for the user button:
```python
button_a = Button(picomotorshim.BUTTON_A)
```

To get the button state, call `.read()`. If the button is held down, then this will return `True` at the interval specified by `repeat_time` until `hold_time` is reached, at which point it will return `True` every `repeat_time / 3` milliseconds. This is useful for rapidly increasing/decreasing values:

```python
state = button_a.read()
```

It is also possible to read the raw button state without the repeat feature, if you prefer:
```python
state = button_a.raw()
```


### Pin Constants

The `motor` module contains a `picomotorshim` sub module with constants for the motor and button pins.


#### Motor Pins

* `MOTOR_1` = `(6, 7)`
* `MOTOR_2` = `(27, 26)`


#### I2C Pins

* `SDA` = `4`
* `SCL` = `5`


#### Button Pin

* `BUTTON_A` = `2`


### Other Constants

The `picomotorshim` sub module also contains a constant for the number of motors on Pico Motor Shim:

* `NUM_MOTORS` = `2`


## Motor 2040

Motor 2040 is a **standalone motor controller** for driving DC motors with encoder feedback. It has JST-SH connectors for plugging in up to 4 motors, with additional bells and whistles like sensor headers, current and voltage monitoring, an RGB LED, and a user button that make it ideal for many robotics projects!

For more information on this board, check out the store page: [pimoroni.com/motor2040](https://pimoroni.com/motor2040).


### Reading the User Button

As is the case for many of Pimoroni's RP2040-based boards, the boot button on Motor 2040 that is used for programming also acts as a user button once a program is running. To simplify the use of this and other buttons, the `pimoroni` module contains a `Button` class that handles button debounce and auto-repeat.

```python
Button(button, invert=True, repeat_time=200, hold_time=1000)
```

To set up the user button, first import the `Button` class from the `pimoroni` module and the pin constant for the button from `motor`:
```python
from pimoroni import Button
from motor import motor2040
```

Then create an instance of `Button` for the user button:
```python
user_sw = Button(motor2040.USER_SW)
```

To get the button state, call `.read()`. If the button is held down, then this will return `True` at the interval specified by `repeat_time` until `hold_time` is reached, at which point it will return `True` every `repeat_time / 3` milliseconds. This is useful for rapidly increasing/decreasing values:

```python
state = user_sw.read()
```

It is also possible to read the raw button state without the repeat feature, if you prefer:
```python
state = user_sw.raw()
```


### Reading the Sensors

On the right-hand edge of Motor 2040 are two analog inputs, with neighbouring 3.3V and GND. These let you connect up sensors to enable your mechanical creations to sense how they are interacting with the world. For example, a pair of analog proximity sensors could be hooked up for wall avoidance or line following, or they could have microswitches wired to report when a motor driven mechanism has reached an end-stop.

Motor 2040 also has six internal sensors:
* A voltage sensor, letting you measure the supply voltage to the motors.
* Four current sensors, letting you measure how much current each motor is drawing.
* A fault sensor, letting you know if there is an issue with one of more of your motors
These could be used just for monitoring, or as the trigger to turn off motors safely when voltage gets too low or current draw gets too high.

To allow for all of these inputs, Motor 2040 has an onboard analog multiplexer that expands a single analog pin into eight, by use of three digital address pins that select a single input at a time. As such, the setup for these sensors is more involved than it would be to just read eight analog pins directly.

To begin reading these inputs, first import the `Analog` and `AnalogMux` classes from `pimoroni` and the pin, address, and gain constants from `motor`:

```python
from pimoroni import Analog
from motor import motor2040
```

Then set up three instances of `Analog` for the sensor and fault, voltage, and current sensing:

```python
sen_adc = Analog(motor2040.SHARED_ADC)
vol_adc = Analog(motor2040.SHARED_ADC, motor2040.VOLTAGE_GAIN)
cur_adc = Analog(motor2040.SHARED_ADC, motor2040.CURRENT_GAIN,
                 motor2040.SHUNT_RESISTOR, motor2040.CURRENT_OFFSET)
```

You may notice, all three of these use the same `SHARED_ADC` pin. This is intentional as it is just a single pin that is being used for all three different functions, only the gains differ.

The next step is to set up the analog multiplexer, by providing it with the three address pins:
```python
mux = AnalogMux(motor2040.ADC_ADDR_0, motor2040.ADC_ADDR_1, motor2040.ADC_ADDR_2)
```
Note that the `AnalogMux` does not know about any of the `Analog` classes that were created before.

With the multiplexer now configured, reading each sensor is simply a case of 'selecting' the sensor on the multiplexer then reading the value from one of the three `Analog` classes created at the start.

To read the two sensor headers:
```python
for addr in range(motor2040.NUM_SENSORS):
    mux.select(addr + motor2040.SENSOR_1_ADDR)
    print("Sensor", addr + 1, "=", sen_adc.read_voltage())
```

To read the voltage sense:
```python
mux.select(motor2040.VOLTAGE_SENSE_ADDR)
print("Voltage =", vol_adc.read_voltage(), "V")
```

To read the current draw in amps (A):
```python
for addr in range(motor2040.NUM_MOTORS):
    mux.select(addr + motor2040.CURRENT_SENSE_A_ADDR)
    print("Current", addr + 1, "=", cur_adc.read_current(), "A")
```


#### Configuring Pulls

For the internal fault sensor, as well as some external sensors, you may need to have the input be pulled high or low before taking a reading. To support this there is an optional `muxed_pin` parameter that can be passed into the `AnalogMux` when creating it, which gives the multiplexer access to the pin to control the pulls.

```python
mux = AnalogMux(motor2040.ADC_ADDR_0, motor2040.ADC_ADDR_1, motor2040.ADC_ADDR_2,
                muxed_pin=Pin(motor2040.SHARED_ADC))
```

From there the pull state of each of the multiplexer's addresses can be configured independently by calling `.configure_pull()`, with the address and the pull state (either `Pin.PULL_UP`, `Pin.PULL_DOWN`, or `None`).

The below example shows how to set both sensor addresses to have pull-downs:
```python
for addr in range(motor2040.NUM_SENSORS):
    mux.configure_pull(addr + motor2040.SENSOR_1_ADDR, Pin.PULL_DOWN)
```

#### Fault Sensing

The drivers on Motor 2040 can detect when there is a fault with their connected motors and shut themselves off for safety. When this occurs they also signal the fault by p