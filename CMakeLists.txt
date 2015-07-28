#=====================================================================================
# ubirch #1 cmake configuration
#=====================================================================================
cmake_minimum_required(VERSION 3.0)


# simulator settings
#set(PROG_TYPE arduino)
#set(PROG_DEV /tmp/simavr-uart0)

# necessary settings for the chip we use
set(MCU   atmega328p)
set(F_CPU 16000000)
set(BAUD  115200)

# set the programmer and the device to use (default is to use simavr)
set(PROG_TYPE     arduino)
# simavr
#set(PROG_DEV     /tmp/simavr-uart0)
# Baite programmer
set(PROG_DEV     /dev/cu.SLAB_USBtoUART)
# the microview programmer
#set(PROG_DEV     /dev/cu.usbserial-DA00UCWL)

set(MONITOR      screen)
set(MONITOR_ARGS ${PROG_DEV} ${BAUD})

# only needed if we do floating point math and want to print floats with printf
#set(EXTRA_LIBS "-lm -lprintf_flt")

# if you want to compile Arduino sketches, set this to the Arduino SDK dir (1.6.5)
set(ARDUINO_SDK_PATH "/Applications/Arduino.app/Contents/Java" CACHE STRING "Arduino SDK Path")

#=====================================================================================
# NO NEED TO CHANGE ANYTHING BELOW
#=====================================================================================

# program names
find_program(AVRCPP   avr-g++)
find_program(AVRC     avr-gcc)
find_program(AVRSTRIP avr-strip)
find_program(OBJCOPY  avr-objcopy)
find_program(OBJDUMP  avr-objdump)
find_program(AVRSIZE  avr-size)
find_program(AVRDUDE  avrdude)

# Sets the compiler
# Needs to come before the project function
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_CXX_COMPILER ${AVRCPP})
set(CMAKE_C_COMPILER   ${AVRC})
set(CMAKE_ASM_COMPILER ${AVRC})

project (ubirch C CXX ASM)

# Important project paths
set(BASE_PATH "${${PROJECT_NAME}_SOURCE_DIR}")
set(SRC_PATH  "${BASE_PATH}/src")
set(LIB_PATH  "${BASE_PATH}/lib")

add_definitions(-DF_CPU=${F_CPU}L)
add_definitions(-DARDUINO=10605)
add_definitions(-DARDUINO_AVR_UNO)
add_definitions(-DARDUINO_ARCH_AVR)

set(COMPILER_FLAGS "-g -Os -Wall -Wextra -MMD -mmcu=${MCU}")

set(CMAKE_C_FLAGS          "${COMPILER_FLAGS} -std=gnu99 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS        "${COMPILER_FLAGS} -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics")
set(CMAKE_ASM_FLAGS        "-x assembler-with-cpp ${COMPILER_FLAGS} ")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")

# we need a little function to add multiple targets
function(add_executable_avr NAME)
    add_executable(${NAME} ${ARGN})
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "${NAME}.elf")
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${NAME}.hex;${NAME}.eep;${NAME}.lst")

    # generate the .hex file
    add_custom_command(
        OUTPUT ${NAME}.hex
        COMMAND ${AVRSTRIP} "${NAME}.elf"
        COMMAND ${OBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 "${NAME}.elf" "${NAME}.eep"
        COMMAND ${OBJCOPY} -O ihex -R .eeprom "${NAME}.elf" "${NAME}.hex"
        COMMAND ${AVRSIZE} --mcu=${MCU} -C --format=avr "${NAME}.elf"
        DEPENDS ${NAME})
    add_custom_target(${NAME}-strip ALL DEPENDS ${NAME}.hex)

    # flash the produces binary
    add_custom_target(
        ${NAME}-flash
        COMMAND ${AVRDUDE} -q -D -c ${PROG_TYPE} -P ${PROG_DEV} -b ${BAUD} -p ${MCU} -U flash:w:${NAME}.hex
        DEPENDS ${NAME}.hex )
    add_custom_target(
        ${NAME}-monitor
        COMMAND ${MONITOR} ${MONITOR_ARGS})
endfunction(add_executable_avr)


# add all the libraries as possible library dependencies
set(PROJECT_LIBS)
file(GLOB LIB_DIRS "${LIB_PATH}/*/CMakeLists.txt")
foreach(cmakedir ${LIB_DIRS})
    get_filename_component(subdir ${cmakedir} PATH)
    if(IS_DIRECTORY ${subdir})
        get_filename_component(target ${subdir} NAME)
        message("FOUND LIBRARY: ${target}")
        add_subdirectory(${subdir})
    endif()
endforeach()

# add targets automatically
file(GLOB SRC_DIRS "${SRC_PATH}/*/CMakeLists.txt")
foreach(cmakedir ${SRC_DIRS})
    get_filename_component(subdir ${cmakedir} PATH)
    if(IS_DIRECTORY ${subdir})
        get_filename_component(target ${subdir} NAME)
        message("FOUND TARGET: ${target}")
        add_subdirectory(${subdir})
    endif()
endforeach()

if(DEFINED ARDUINO_SDK_PATH AND IS_DIRECTORY ${ARDUINO_SDK_PATH})
    set(ARDUINO_CORES_PATH ${ARDUINO_SDK_PATH}/hardware/arduino/avr/cores/arduino)
    set(ARDUINO_VARIANTS_PATH ${ARDUINO_SDK_PATH}/hardware/arduino/avr/variants/standard)
    set(ARDUINO_LIBRARIES_PATH ${ARDUINO_SDK_PATH}/hardware/arduino/avr/libraries)

    file(GLOB_RECURSE ARDUINO_CORES_SRCS ${ARDUINO_CORES_PATH}/*.S ${ARDUINO_CORES_PATH}/*.c ${ARDUINO_CORES_PATH}/*.cpp)
    add_library(arduino-core ${ARDUINO_CORES_SRCS})
    target_include_directories(arduino-core PUBLIC ${ARDUINO_CORES_PATH})
    target_include_directories(arduino-core PUBLIC ${ARDUINO_VARIANTS_PATH})

    file(GLOB CORE_DIRS ${ARDUINO_LIBRARIES_PATH}/*)
    set(LIBS)
    foreach(libdir ${CORE_DIRS})
        get_filename_component(libname ${libdir} NAME)
        if(IS_DIRECTORY ${libdir})
            file(GLOB_RECURSE sources ${libdir}/*.cpp ${libdir}/*.S ${libdir}/*.c )
            string(REGEX REPLACE "examples/.*" "" sources "${sources}")
            if(sources)
                message("FOUND LIB: ${libname}")
                add_library(${libname} ${sources})
                target_link_libraries(${libname} arduino-core)
                foreach(src ${sources})
                    get_filename_component(dir ${src} PATH)
                    target_include_directories(${libname} PUBLIC ${dir})
                endforeach()
                list(APPEND LIBS ${libname})
            else()
                message("FOUND INC: ${libname}")
                include_directories(SYSTEM ${libdir})
            endif()
        endif()
    endforeach()

    file(GLOB EXT_DIRS external/*)
    foreach(libdir ${EXT_DIRS})
        get_filename_component(libname ${libdir} NAME)
        if(IS_DIRECTORY ${libdir})
            file(GLOB_RECURSE sources ${libdir}/*.asm ${libdir}/*.c ${libdir}/*.cpp )
            string(REGEX REPLACE "examples/.*" "" sources "${sources}")
            if(sources)
                message("FOUND EXT LIB: ${libname}")
                add_library(${libname} ${sources})
                target_link_libraries(${libname} ${LIBS})
                foreach(src ${sources})
                    get_filename_component(dir ${src} PATH)
                    target_include_directories(${libname} PUBLIC ${dir})
                endforeach()
            else()
                message("FOUND INC: ${libname}")
                include_directories(SYSTEM ${libdir})
            endif()
            list(APPEND LIBS ${libname})
        endif()
    endforeach()
else()
    message(WARNING "No Arduino SDK found at '${ARDUINO_SDK_PATH}', can't compile sketches!")
endif()