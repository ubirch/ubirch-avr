add_executable_avr(fewl fewl.cpp)
# we require a number of arduino libraries for this
# install these libraries and either copy them or create a link in the 'external' directory
target_link_libraries(fewl SoftwareSerial Wire Adafruit_FONA_Library Adafruit_SleepyDog ISL29125_Breakout)