# Eliza on the ubirch #1

This little program puts Eliza on the board. Why? Because I wanted to learn how to
program the Arduino compatible board without the IDE using just the standard toolchain
[avr-libc](http://www.nongnu.org/avr-libc/) and [cmake](http://www.cmake.org/).

Another goal was to learn about how to set up the serial communication to work with
the standard tools, like puts(), fgets(), printf(), etc. As a side effect I even learned
something about [PROGMEM](http://www.fourwalledcubicle.com/AVRArticles.php).

## to load the code onto the board 

1. Edit CMakeLists.txt and set PROG_TYPE and PROG_DEV to your flash programmer and device.
2. follow the following procedure:

```
mkdir build
cd build
cmake ..
make flash monitor
```

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
