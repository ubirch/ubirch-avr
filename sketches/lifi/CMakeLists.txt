add_executable_avr(lifi lifi.cpp)
target_link_libraries(lifi SoftwareSerial)
target_sketch_library(lifi Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel)

