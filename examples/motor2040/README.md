# Motor 2040 C++ Examples <!-- omit in toc -->

- [Motor Examples](#motor-examples)
  - [Single Motor](#single-motor)
  - [Multiple Motors](#multiple-motors)
  - [Motor Cluster](#motor-cluster)
  - [Motor Wave](#motor-wave)
- [Function Examples](#function-examples)
  - [Read Sensors](#read-sensors)
  - [Read Encoders](#read-encoders)
  - [Motor Profiler](#motor-profiler)
  - [LED Rainbow](#led-rainbow)
  - [Turn Off LEDs](#turn-off-leds)
- [Control Examples](#control-examples)
  - [Position Control](#position-control)
  - [Velocity Control](#velocity-control)
  - [Position on Velocity Control](#position-on-velocity-control)
  - [Reactive Encoder](#reactive-encoder)
  - [Quad Position Wave](#quad-position-wave)
  - [Quad Velocity Sequence](#quad-velocity-sequence)
- [Tuning Examples](#tuning-examples)
  - [Position Tuning](#position-tuning)
  - [Velocity Tuning](#velocity-tuning)
  - [Position on Velocity Tuning](#position-on-velocity-tuning)


## Motor Examples

### Single Motor
[motor2040_single_motor.cpp](motor2040_single_motor.cpp)

Demonstrates how to create a Motor object and control it.


### Multiple Motors
[motor2040_multiple_motors.cpp](motor2040_multiple_motors.cpp)

Demonstrates how to create multiple Motor objects and control them together.


### Motor Cluster
[motor2040_motor_cluster.cpp](motor2040_motor_cluster.cpp)

Demonstrates how to create a MotorCluster object to control multiple motors at once.


### Motor Wave
[motor2040_motor_wave.cpp](motor2040_motor_wave.cpp)

An example of applying a wave pattern to a group of motors and the LEDs.


## Function Examples

### Read Sensors
[motor2040_read_sensors.cpp](motor2040_read_sensors.cpp)

Shows how to initialise and read the 2 external and 6 internal sensors of Motor 2040.


### Read Encoders
[motor2040_read_encoders.cpp](motor2040_read_encoders.cpp)

Demonstrates how to read the angles of Motor 2040's four encoders.


### Motor Profiler
[motor2040_motor_profiler.cpp](motor2040_motor_profiler.cpp)

A program that profiles the speed of a motor across its PWM
duty cycle range using the attached enc