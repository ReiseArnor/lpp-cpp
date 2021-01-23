set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 

set(catch2 catch.hpp) 

add_library(catch SHARED ${catch2}) 
set_target_properties(catch PROPERTIES LINKER_LANGUAGE CXX) 

target_include_directories(catch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})