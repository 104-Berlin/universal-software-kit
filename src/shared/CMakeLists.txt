cmake_minimum_required(VERSION 3.15)
project(USKShared)

add_library(Shared SHARED)
target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(logging/spdlog)
target_link_libraries(spdlog)