#=====================================================================================
# ubirch #1 cmake configuration
#=====================================================================================
cmake_minimum_required(VERSION 2.8)

# set the programmer and the device to use (default is to use simavr)
#set(PROG_TYPE arduino)
#set(PROG_DEV /dev/cu.SLAB_USBtoUART)

# simulator settings
set(PROG_TYPE arduino)
set(PROG_DEV /tmp/simavr-uart0)

# necessary settings for the chip we use
set(MCU   atmega328p)
set(F_CPU 16000000UL)
set(BAUD  19200UL)

# only needed if we do floating point math and want to print floats with printf
#set(EXTRA_LIBS "-lm -lprintf_flt")

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
find_program(MONITOR  screen)

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

# Compiler flags
set(CSTANDARD "-std=gnu99")
set(CDEBUG    "-gstabs -g -ggdb")
set(CWARN     "-Wall -Wstrict-prototypes -Wno-unknown-pragmas -Wl,--gc-sections -Wl,--relax")
set(CTUNING   "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
set(COPT      "-Os ${EXTRA_LIBS}")
set(CMCU      "-mmcu=${MCU}")
set(CDEFS     "-DF_CPU=${F_CPU} -DBAUD=${BAUD}")

set(CFLAGS   "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CXXFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CTUNING} -fno-threadsafe-statics")

set(CMAKE_C_FLAGS   "${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")
set(CMAKE_ASM_FLAGS   "${SSCFLAGS}")

# we need a little function to add multiple targets
function(add_executable_avr NAME)
    add_executable(${NAME} ${ARGN})
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "${NAME}.elf")

    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${NAME}.hex;${NAME}.eeprom;${NAME}.lst")

    # generate the .hex file
    add_custom_command(
        OUTPUT ${NAME}.hex
        COMMAND ${AVRSTRIP} "${NAME}.elf"
        COMMAND ${OBJCOPY} -R .eeprom -O ihex "${NAME}.elf" "${NAME}.hex"
        COMMAND ${OBJCOPY} -j .eeprom --change-section-lma .eeprom=0 -O ihex "${NAME}.elf" "${NAME}.eeprom"
        COMMAND ${AVRSIZE} --mcu=${MCU} -C --format=avr "${NAME}.elf"
        DEPENDS ${NAME}
    )
    add_custom_target(${NAME}-strip ALL DEPENDS ${NAME}.hex)

    # flash the produces binary
    add_custom_target(
        ${NAME}-flash
        COMMAND ${AVRDUDE} -q -D -c ${PROG_TYPE} -P ${PROG_DEV} -p ${MCU} -U flash:w:${NAME}.hex
        DEPENDS ${NAME}.hex
    )
endfunction(add_executable_avr)

add_custom_target(monitor ${MONITOR} ${PROG_DEV} ${BAUD})

# add all the libraries as possible library dependencies
set(PROJECT_LIBS)
file(GLOB LIB_DIRS "${LIB_PATH}/*/CMakeLists.txt")
foreach(cmakedir ${LIB_DIRS})
    get_filename_component(subdir ${cmakedir} PATH)
    if(IS_DIRECTORY ${subdir})
       message("FOUND LIBRARY: ${subdir}")
       add_subdirectory(${subdir})
    endif()
endforeach()

# add targets automatically
file(GLOB SRC_DIRS "${SRC_PATH}/*/CMakeLists.txt")
foreach(cmakedir ${SRC_DIRS})
    get_filename_component(subdir ${cmakedir} PATH)
    if(IS_DIRECTORY ${subdir})
        message("FOUND TARGET: ${subdir}")
        add_subdirectory(${subdir})
    endif()
endforeach()