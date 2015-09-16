add_executable_avr(serial serial.c)
target_link_libraries(serial suart uart dbg_utils)