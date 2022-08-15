cmake_minimum_required(VERSION 3.15)
project(USKContentServer)

add_executable(EContentServer server_entry.cpp)

target_include_directories(EContentServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(EContentServer Shared)