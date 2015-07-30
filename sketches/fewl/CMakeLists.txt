add_executable_avr(fewl fewl.cpp)
# we require a number of arduino libraries for this
# (they already depend on arduino-core, so we don't need it here)
# install these libraries and either copy them or create a link in the 'external' directory
target_link_libraries(fewl SoftwareSerial Adafruit_FONA_Library Adafruit_SleepyDog ISL29125_Breakout)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
    message(AUTHOR_WARNING "In directory 'sketches/fewl': installing the template config.h, please edit!")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.template ${CMAKE_CURRENT_SOURCE_DIR}/config.h COPYONLY)
endif()

# try to install dependent libraries
find_package(Git)
if(GIT_FOUND)
    set(MYLIBS ${CMAKE_CURRENT_SOURCE_DIR}/../libraries)
    if(NOT EXISTS ${MYLIBS}/Adafruit_FONA_Library)
        message(STATUS "Installing Adafruit FONA library")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/adafruit/Adafruit_FONA_Library
            WORKING_DIRECTORY ${MYLIBS})
    endif()
    if(NOT EXISTS ${MYLIBS}/Adafruit_SleepyDog)
        message(STATUS "Installing Adafruit SleepyDog library")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/adafruit/Adafruit_SleepyDog
            WORKING_DIRECTORY ${MYLIBS})
    endif()
    if(NOT EXISTS ${MYLIBS}/ISL29125_Breakout)
        message(STATUS "Installing SparkFun ISL29125 Breakout library")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone https://github.com/sparkfun/ISL29125_Breakout
            WORKING_DIRECTORY ${MYLIBS})
    endif()
else()
    message(FATAL_ERROR "Missing git, please install and try again!")
endif()