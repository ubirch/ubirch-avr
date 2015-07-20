add_executable_avr(twitest twitest.c)
target_link_libraries(twitest i2c uart dbg_utils)