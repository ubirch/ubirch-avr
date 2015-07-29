add_executable_avr(fewl fewl.cpp)
# we require a number of arduino libraries for this
# (they already depend on arduino-core, so we don't need it here)
# install these libraries and either copy them or create a link in the 'external' directory
target_link_libraries(fewl SoftwareSerial Adafruit_FONA_Library Adafruit_SleepyDog ISL29125_Breakout)
