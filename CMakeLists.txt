cmake_minimum_required(VERSION 2.8)

#set(PROG_TYPE arduino)
#set(PROG_DEV /dev/cu.SLAB_USBtoUART)

# simulator settings
set(PROG_TYPE arduino)
set(PROG_DEV /tmp/simavr-uart0)

# Variables regarding the AVR chip
set(MCU   atmega328p)
set(F_CPU 16000000UL)
set(BAUD  19200UL)
add_definitions(-DF_CPU=${F_CPU})

# only needed if we do floating point math and want to print floats with printf
#set(EXTRA_LIBS "-lm -lprintf_flt")

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
set(BASE_PATH    "${${PROJECT_NAME}_SOURCE_DIR}")
set(INC_PATH     "${BASE_PATH}/include")
set(SRC_PATH     "${BASE_PATH}/src")
set(LIB_DIR_PATH "${BASE_PATH}/lib")

# Files to be compiled
file(GLOB SRC_FILES "${SRC_PATH}/*.cpp"
                    "${SRC_PATH}/*.cc"
                    "${SRC_PATH}/*.c"
                    "${SRC_PATH}/*.cxx"
                    "${SRC_PATH}/*.S"
                    "${SRC_PATH}/*.s"
                    "${SRC_PATH}/*.sx"
                    "${SRC_PATH}/*.asm")


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

set(PROJECT_LIBS)
file(GLOB LIB_DIRS "${LIB_DIR_PATH}/*")
foreach(subdir ${LIB_DIRS})
    add_subdirectory(${subdir})
    if(IS_DIRECTORY ${subdir})
       get_filename_component(NAME ${subdir} NAME)
       list(APPEND PROJECT_LIBS ${NAME})
    endif()
endforeach()

# Project setup
include_directories(${INC_PATH} ${LIB_INC_PATH})
add_executable(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC ${PROJECT_LIBS})

# Compiling targets
add_custom_target(strip  ALL ${AVRSTRIP} "${PROJECT_NAME}.elf" DEPENDS ${PROJECT_NAME})
add_custom_target(hex    ALL ${OBJCOPY} -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS strip)
add_custom_target(eeprom     ${OBJCOPY} -j .eeprom --change-section-lma .eeprom=0 -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.eeprom" DEPENDS strip)
add_custom_target(size       ${AVRSIZE} --mcu=${MCU} -C --format=avr "${PROJECT_NAME}.elf" DEPENDS hex)

add_custom_target(flash      ${AVRDUDE} -q -D -c ${PROG_TYPE} -P ${PROG_DEV} -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS size)
add_custom_target(monitor    ${MONITOR} ${PROG_DEV} ${BAUD} DEPENDS hex)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")