add_executable_avr(chatbot chatbot.c)
target_link_libraries(chatbot eliza uart)