set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(lexer_sources   tests_main.cpp 
                    lexer_test.cpp 
                    ../lexer.cpp)
set(parser_sources  tests_main.cpp 
                    parser_test.cpp 
                    ../lexer.cpp
                    ../parser.cpp)

add_executable(lexer_tests ${lexer_sources})
add_executable(parser_tests ${parser_sources})

target_link_libraries(lexer_tests catch)
target_link_libraries(parser_tests catch)

add_test(lexer lexer_tests)
add_test(parser parser_tests)