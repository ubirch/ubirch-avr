add_executable_avr(pushbutton pushbutton.cpp)
# we require a number of arduino libraries for this
# (they already depend on arduino-core, so we don't need it here)
# install these libraries and either copy them or create a link in the 'external' directory
target_link_libraries(pushbutton SoftwareSerial)

# special external dependencies can be added like this and will be downloaded once
# arguments: <target> <name> <git url>
target_sketch_library(pushbutton Adafruit_FONA_Library https://github.com/adafruit/Adafruit_FONA_Library)
target_sketch_library(pushbutton Adafruit_SleepyDog https://github.com/adafruit/Adafruit_SleepyDog)
target_sketch_library(pushbutton FastLED https://github.com/FastLED/FastLED)

