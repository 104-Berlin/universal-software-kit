cmake_minimum_required(VERSION 3.15)
project(ECS)

add_library(EntityModel )

target_include_directories(EntityModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})