cmake_minimum_required(VERSION 3.15)
project(USKMemory-Test)

add_executable(MemTest mem_test.cpp)
target_link_libraries(MemTest EInterface)