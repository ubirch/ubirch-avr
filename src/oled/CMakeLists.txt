add_executable_avr(oled oled.c)
target_link_libraries(oled isl29125 i2c uart dbg_utils)