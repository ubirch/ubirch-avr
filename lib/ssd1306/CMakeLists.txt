add_library(ssd1306 ssd1306.c)
target_link_libraries(ssd1306 i2c dbg_utils)
target_include_directories(ssd1306 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

