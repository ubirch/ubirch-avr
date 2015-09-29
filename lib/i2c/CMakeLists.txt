add_library(i2c i2c_core.c i2c_registers.c)
target_link_libraries(i2c dbg_utils)
target_include_directories(i2c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

