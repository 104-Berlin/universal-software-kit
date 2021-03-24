cmake_minimum_required(VERSION 3.15)
project(ECS)

add_library(EntityModel engine_object.cpp
                        engine_uuid.cpp
                        engine_scene.cpp)

target_include_directories(EntityModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})