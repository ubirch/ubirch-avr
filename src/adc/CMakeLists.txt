add_executable_avr(adc adc.c)
target_link_libraries(adc uart dbg_utils)