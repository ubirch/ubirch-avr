# necessary settings for the chip we use
set(MCU attiny45)
set(F_CPU 8000000)
set(BAUD 9600)

# set the programmer and the device to use (default is to use simavr)
set(PROGRAMMER usbasp)
set(SERIAL_DEV /dev/cu.SLAB_USBtoUART)
#set(PROGRAMMER arduino)
#set(SERIAL_DEV /tmp/simavr-uart0)

# only needed if we do floating point math and want to print floats with printf
#set(EXTRA_LIBS "-lm -lprintf_flt")
# show a list of libs linked
set(EXTRA_LIBS "-Wl,--trace")

# if you want to compile Arduino sketches, set this to the Arduino SDK dir (1.6.5)
set(ARDUINO_SDK_PATH "/Applications/Arduino.app/Contents/Java" CACHE STRING "Arduino SDK Path")