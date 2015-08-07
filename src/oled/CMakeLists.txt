add_executable_avr(oled oled.c)
target_link_libraries(oled isl29125 ssd1306 uart dbg_utils)