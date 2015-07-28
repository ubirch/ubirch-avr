#!/bin/bash -x

# this files helps bundling the build commands

mkdir -p build && cd build && cmake -DARDUIN_SDK_PATH=$ARDUINO_SDK_PATH .. && make
