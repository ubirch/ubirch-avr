# Load Arduino sketches

if(DEFINED ENV{ARDUINO_SDK_PATH})
    set (ARDUINO_SDK_PATH "$ENV{ARDUINO_SDK_PATH}" CACHE PATH "Arduino SDK Path" FORCE)
endif()
if(DEFINED ARDUINO_SDK_PATH AND IS_DIRECTORY ${ARDUINO_SDK_PATH})

    # set the paths
    set(ARDUINO_CORES_PATH ${ARDUINO_SDK_PATH}/hardware/arduino/avr/cores/arduino)
    set(ARDUINO_VARIANTS_PATH ${ARDUINO_SDK_PATH}/hardware/arduino/avr/variants/standard)
    set(ARDUINO_LIBRARIES_PATH ${ARDUINO_SDK_PATH}/hardware/arduino/avr/libraries)

    # configure the core library
    file(GLOB_RECURSE ARDUINO_CORES_SRCS ${ARDUINO_CORES_PATH}/*.S ${ARDUINO_CORES_PATH}/*.c ${ARDUINO_CORES_PATH}/*.cpp)
    add_library(arduino-core ${ARDUINO_CORES_SRCS})
    target_include_directories(arduino-core PUBLIC ${ARDUINO_CORES_PATH})
    target_include_directories(arduino-core PUBLIC ${ARDUINO_VARIANTS_PATH})

    # configure all the additional libraries in the core
    file(GLOB CORE_DIRS ${ARDUINO_LIBRARIES_PATH}/*)
    set(LIBS)
    foreach(libdir ${CORE_DIRS})
        get_filename_component(libname ${libdir} NAME)
        if(IS_DIRECTORY ${libdir})
            file(GLOB_RECURSE sources ${libdir}/*.cpp ${libdir}/*.S ${libdir}/*.c )
            string(REGEX REPLACE "examples/.*" "" sources "${sources}")
            if(sources)
                message(STATUS "Adding Arduino Core library: ${libname}")
                add_library(${libname} ${sources})
                target_link_libraries(${libname} arduino-core)
                foreach(src ${sources})
                    get_filename_component(dir ${src} PATH)
                    target_include_directories(${libname} PUBLIC ${dir})
                endforeach()
                list(APPEND LIBS ${libname})
            else()
                include_directories(SYSTEM ${libdir})
            endif()
        endif()
    endforeach()

    # add a sketch dependency by giving the target and a git url
    function(target_sketch_library TARGET NAME URL)
        # try to install dependent libraries
        find_package(Git)
        if(GIT_FOUND)
            # clone dependency into libraries
            get_filename_component(MYLIBS "${CMAKE_CURRENT_SOURCE_DIR}/../libraries" REALPATH)
            message("${MYLIBS}")
            if(NOT EXISTS ${MYLIBS}/${NAME})
                message(STATUS "Installing ${NAME} (${URL})")
                execute_process(
                    COMMAND ${GIT_EXECUTABLE} clone "${URL}" "${NAME}"
                    WORKING_DIRECTORY ${MYLIBS})
            endif()

            # now add a library target
            file(GLOB_RECURSE sources ${MYLIBS}/${NAME}/*.S ${MYLIBS}/${NAME}/*.c ${MYLIBS}/${NAME}/*.cpp )
            string(REGEX REPLACE "examples/.*" "" sources "${sources}")
            if(sources)
                message(STATUS "Adding external library: ${NAME}")
                add_library(${NAME} ${sources})
                target_link_libraries(${NAME} ${LIBS})
                foreach(src ${sources})
                    get_filename_component(dir ${src} PATH)
                    target_include_directories(${NAME} PUBLIC ${dir})
                endforeach()
            else()
                message(FATAL_ERROR "Could not find sources in library ${NAME}!")
            endif()
        else()
            message(FATAL_ERROR "Missing git, please install and try again!")
        endif()
        target_link_libraries(${TARGET} ${NAME})
    endfunction(target_sketch_library)

    # finally add all the sketches
    file(GLOB SRC_DIRS "*/CMakeLists.txt")
    foreach(cmakedir ${SRC_DIRS})
        get_filename_component(subdir ${cmakedir} PATH)
        if(IS_DIRECTORY ${subdir})
            get_filename_component(target ${subdir} NAME)
            message(STATUS "Found Arduino sketch: ${target}")
            add_subdirectory(${subdir})
        endif()
    endforeach()

else()
    message(WARNING "No Arduino SDK found at '${ARDUINO_SDK_PATH}', can't compile sketches!")
endif()