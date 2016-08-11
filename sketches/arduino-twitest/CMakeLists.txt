add_executable_avr(arduino-twitest twitest.cpp)
target_link_libraries(arduino-twitest SoftwareSerial Wire)
