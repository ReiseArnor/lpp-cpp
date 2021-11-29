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

set(eval_sources    tests_main.cpp
                    evaluator_test.cpp
                    ../lexer.cpp
                    ../parser.cpp)

add_executable(lexer_tests ${lexer_sources})
add_executable(parser_tests ${parser_sources})
add_executable(ast_tests ${ast_sources})
add_executable(eval_tests ${eval_sources})

target_link_libraries(lexer_tests PRIVATE ${CONAN_LIBS})
target_link_libraries(parser_tests PRIVATE ${CONAN_LIBS})
target_link_libraries(ast_tests PRIVATE ${CONAN_LIBS})
target_link_libraries(eval_tests PRIVATE ${CONAN_LIBS})

add_test(lexer ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lexer_tests)
add_test(parser ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/parser_tests)
add_test(ast ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ast_tests)
add_test(evaluator ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/eval_tests)
