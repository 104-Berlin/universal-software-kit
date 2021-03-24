cmake_minimum_required(VERSION 3.15)
project(USK)

# GLOBAL SETTINGS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src/)



add_subdirectory(src/ecs)


add_library(USKShared src/engine_shared.cpp)

target_link_libraries(EntityModel)