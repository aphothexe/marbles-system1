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
    - [Direction](#direction