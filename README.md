# ubirch #1 AVR lib

Contains little libs to work with AVR chip internals as well as to speak with peripherpherals.

## Building
 
### Prerequisits

Install the toolchain:

For MacOS X the simplest way to get the toolchain is [homebew](http://brew.sh/):

```
brew tap larsimmisch/avr
brew install avr-binutils avr-gcc avr-libc avrdude
brew install cmake
```

### Make

1. Edit CMakeLists.txt and set PROG_TYPE and PROG_DEV to your flash programmer and device.
2. follow the following procedure:

```
mkdir build
cd build
cmake ..
make flash monitor
```

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
