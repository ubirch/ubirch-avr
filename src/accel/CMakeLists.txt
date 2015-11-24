add_executable_avr(accel accel.c)
target_link_libraries(accel i2c uart dbg_utils)