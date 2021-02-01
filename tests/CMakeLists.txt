set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(lexer_sources   tests_main.cpp 
                    lexer_test.cpp 
                    ../lexer.cpp)

set(parser_sources  tests_main.cpp 
                    parser_test.cpp 
                    ../lexer.cpp
                    ../parser.cpp)

set(ast_sources     tests_main.cpp
                    ast_test.cpp
                    ../lexer.cpp
                    ../parser.cpp)

add_executable(lexer_tests ${lexer_sources})
add_executable(parser_tests ${parser_sources})
add_executable(ast_tests ${ast_sources})

Include(FetchContent)

FetchContent_Declare(
Catch2
GIT_REPOSITORY https://github.com/catchorg/Catch2.git
GIT_TAG        v2.13.4)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(lexer_tests PRIVATE Catch2::Catch2)
target_link_libraries(parser_tests PRIVATE Catch2::Catch2)
target_link_libraries(ast_tests PRIVATE Catch2::Catch2)

add_test(lexer lexer_tests)
add_test(parser parser_tests)
add_test(ast ast_tests)