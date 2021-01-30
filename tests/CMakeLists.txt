set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(lexer_sources tests_main.cpp lexer_test.cpp ../lexer.cpp)
add_executable(tests ${lexer_sources})
target_link_libraries(tests catch)
add_test(lexer tests)