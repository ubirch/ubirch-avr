add_library(mpr121 mpr121.c)
target_link_libraries(mpr121 i2c dbg_utils)
target_include_directories(mpr121 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

