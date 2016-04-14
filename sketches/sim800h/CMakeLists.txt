add_executable_avr(sim800h sim800h.cpp)
# we require a number of arduino libraries for this
# (they already depend on arduino-core, so we don't need it here)
# install these libraries and either copy them or create a link in the 'external' directory
target_link_libraries(sim800h SoftwareSerial)
target_sketch_library(sim800h UbirchSIM800 "https://github.com/ubirch/ubirch-avr-sim800")

# copy the config.h.template to config.h in case it is not there; it is ignored by .git!
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
    message(AUTHOR_WARNING "In directory 'sketches/sim800h': installing the template config.h, please edit!")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.template ${CMAKE_CURRENT_SOURCE_DIR}/config.h COPYONLY)
endif ()
