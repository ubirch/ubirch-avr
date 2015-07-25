add_library(isl29125 isl29125.c)
target_link_libraries(isl29125 i2c dbg_utils)
target_include_directories(isl29125 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

