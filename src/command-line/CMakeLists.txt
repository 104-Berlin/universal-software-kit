cmake_minimum_required(VERSION 3.15)
project(USKCommandLine)

add_executable(ECommandLine clt_entry.cpp)

target_include_directories(ECommandLine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ECommandLine EInterface)