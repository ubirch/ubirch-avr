add_executable_avr(morse morse.c)
target_link_libraries(morse uart dbg_utils)