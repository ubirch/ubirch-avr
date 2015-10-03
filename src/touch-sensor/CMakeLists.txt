add_executable_avr(touch-sensor touch_sensor.c)
target_link_libraries(touch-sensor mpr121 i2c uart dbg_utils)