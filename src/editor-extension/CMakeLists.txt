cmake_minimum_required(VERSION 3.15)

project(USKEditorExtension)

add_library(EEditorExtension SHARED ui/editor_ext_ui.cpp)

target_compile_definitions(EEditorExtension PRIVATE EXPORT_EDITOR_EXTENSION)
target_include_directories(EEditorExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

