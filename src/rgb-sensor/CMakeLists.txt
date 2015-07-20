add_executable_avr(rgb_sensor rgb_sensor.c)
target_link_libraries(rgb_sensor i2c uart dbg_utils)