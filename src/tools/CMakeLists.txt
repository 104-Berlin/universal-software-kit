cmake_minimum_required(VERSION 3.15)
project(ETools)

add_executable(USKCompileFile compile_file.cpp)

target_include_directories(USKCompileFile PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(USKCompileFile Util)