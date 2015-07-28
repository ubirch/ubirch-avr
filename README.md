# ubirch #1 AVR lib

Contains little libs to work with AVR chip internals as well as to speak with peripherpherals.
Additionally allows for compiling sketches that have been renamed ```sketch-name.cpp``` and
have fixed paths. Please read the CMakeLists.txt examples to understand how to tell the build
system which libraries your code needs.

## Building
 
### Prerequisits

Install the toolchain:

For MacOS X the simplest way to get the toolchain is [homebew](http://brew.sh/):

```
brew tap larsimmisch/avr
brew install avr-binutils avr-gcc avr-libc avrdude
brew install cmake
```

If you want to be able to compile [Arduino](https://www.arduino.cc/) sketches you will also need
the [Arduino SDK](https://www.arduino.cc/en/Main/Software) and set the environment variable ARDUINO_SDK_PATH.

### Make

1. Edit CMakeLists.txt and set PROG_TYPE and PROG_DEV to your flash programmer and device.
2. follow the following procedure:

```
mkdir build
cd build
cmake ..
make eliza-flash monitor
```

To flash a target use the directory name with added -flash (twitest-flash for the TWI example).

> Unless you make significant changes to the CMakeLists.txt you only need to run ```make``` in
> the build directory from now on.

## SimAVR (testing code)

To run code inside of [SIMAVR](https://github.com/buserror/simavr), just download and install simavr as well as the simduino
example:

```
git clone git@github.com:buserror/simavr.git
cd simavr
make
cd examples/board_simduino
./obj-x86_64-apple-darwin14.4.0/simduino.elf 
```

Now the simulator is running with the Arduino bootloader code and it tells us the tty we can
use to connect and flash code on to the virtual device:

```
read_ihex_chunks: ATmegaBOOT_168_atmega328.ihex, unsupported check type 03
atmega328p booloader 0x07800: 1950 bytes
avr_interrupt_init
avr_special_init
uart_pty_init bridge on port *** /dev/ttys005 ***
uart_pty_connect: /tmp/simavr-uart0 now points to /dev/ttys005
```

Edit ```CMakeLists.txt``` to set the correct PROG_DEV and then just follow the instructions above.

## Library Code

### debug

This lib mainly contains some macros that help debugging and will not be compiled into your
code if you compile for release.

### eliza

This is [code](https://github.com/itmm/eliza)
I found online which I rewrote to work with the limited capacity of the atmega328p by
moving most of the strings into [PROGMEM](http://www.fourwalledcubicle.com/AVRArticles.php).
 
### i2c

Library to access the [TWI](http://www.nongnu.org/avr-libc/user-manual/group__twi__demo.html)
(two wire interface, or i2c) interface in the AVR. Has the most basic functions to initialize
the bus and send and receive data.

### isl29125

Library that talks to the [ISL29125](http://www.intersil.com/content/dam/Intersil/documents/isl2/isl29125.pdf)
RGB sensor via i2c. Also contains code to downsample the 48bit color depth to a more useful 24bit.

### uart

Basic communication with the board is done using the the UART. This library is basically
the [demo code](http://www.nongnu.org/avr-libc/user-manual/group__stdiodemo.html) from
the [avr-libc](http://www.nongnu.org/avr-libc/).

## Main Targets
 
There are a number of executables available under ```src```. These are described below: 

### Eliza

This little program puts Eliza on the board. Why? Because I wanted to learn how to
program the Arduino compatible board without the IDE using just the standard toolchain
[avr-libc](http://www.nongnu.org/avr-libc/) and [cmake](http://www.cmake.org/).

Another goal was to learn about how to set up the serial communication to work with
the standard tools, like puts(), fgets(), printf(), etc. As a side effect I even learned
something about [PROGMEM](http://www.fourwalledcubicle.com/AVRArticles.php).

Enjoy a little chat with Eliza!

### TWI Test

The TWI (two wire interface, also known as i2c) is a short test program that scans the i2c
bus for devices and outputs the device id.

### RGB Sensor Example

Using the i2c library, this example talks to an [ISL29125 RGB sensor](https://www.sparkfun.com/products/12829)
and reads the data via i2c and displays the results on the serial console.

### OLED Breakout

The [OLED Breakout](https://www.sparkfun.com/products/13003) is a nice little display also found in
the [MicroView](http://microview.io/). In this example a few methods of accessing the display via i2c
are explored and coupled with the RGB sensor.

## LICENSE

    Copyright 2015 ubirch GmbH (http://www.ubirch.com)
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
