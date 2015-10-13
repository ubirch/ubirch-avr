#! /bin/sh
avrdude -v -v -p atmega328p -c dragon_isp -P usb -B 10 -U hfuse:w:0x9f:m
