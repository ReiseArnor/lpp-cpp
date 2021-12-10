cmake_minimum_required(VERSION 3.9)
project(lpp_interpreter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if(MSVC)
  set(CPP_FLAGS /W4 /permissive- /fsanitize=address)
  set(CPP_LINKING_OPTS /fsanitize=address)
else()
    set(CPP_FLAGS -fsanitize=undefined,address, -fno-omit-frame-pointer -Wall -Wextra -Wconversion -Wpedantic)
    set (CPP_LINKING_OPTS -fno-omit-frame-pointer -fsanitize=undefined,address)
endif()

add_executable(${PROJECT_NAME} main.cpp repl.cpp parser.cpp lexer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::fmt)
target_compile_options(${PROJECT_NAME} PRIVATE ${CPP_FLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${CPP_LINKING_OPTS})

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
