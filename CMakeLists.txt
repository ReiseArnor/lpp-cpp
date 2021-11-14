cmake_minimum_required(VERSION 3.9)
project(lpp_interpreter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC)
  set(CPP_FLAGS /W4)
else()
  set(CPP_FLAGS -Wall -Wextra -Wconversion)
endif()

add_executable(${PROJECT_NAME} main.cpp repl.cpp parser.cpp lexer.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE ${CPP_FLAGS})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    enable_testing()
    add_subdirectory(tests)
endif()

if(MSVC)
  add_compile_options("$<$<CONFIG:DEBUG>:/DEBUG;/Od>")
  add_compile_options("$<$<CONFIG:RELEASE>:/O2")
else()
  add_compile_options("$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>")
  add_compile_options("$<$<CONFIG:RELEASE>:-O3")
endif()
