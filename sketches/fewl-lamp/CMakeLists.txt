add_executable_avr(lamp lamp.cpp)
# we require a number of arduino libraries for this
# (they already depend on arduino-core, so we don't need it here)
# install these libraries and either copy them or create a link in the 'external' directory
target_link_libraries(lamp SoftwareSerial)

# special external dependencies can be added like this and will be downloaded once
# arguments: <target> <name> <git url>
target_sketch_library(lamp UbirchSIM800 "https://github.com/ubirch/ubirch-avr-sim800")
target_sketch_library(lamp Adafruit_SleepyDog https://github.com/adafruit/Adafruit_SleepyDog)
target_sketch_library(lamp Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel)

# copy the config.h.template to config.h in case it is not there; it is ignored by .git!
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
    message(AUTHOR_WARNING "In directory 'sketches/lamp': installing the template config.h, please edit!")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.template ${CMAKE_CURRENT_SOURCE_DIR}/config.h COPYONLY)
endif ()

