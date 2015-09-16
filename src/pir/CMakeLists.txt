add_executable_avr(pir pir.c)
target_link_libraries(pir uart dbg_utils)