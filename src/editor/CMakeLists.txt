cmake_minimum_required(VERSION 3.15)
project(USKEditor)

add_executable(USKEditor    editor_entry.cpp
                            ui-fields/editor_ui_fields.cpp
                            ui-fields/editor_basic_object_view.cpp
                            ui-fields/editor_resource_view.cpp
                            ui-fields/editor_basic_register_view.cpp
                            ui-fields/editor_extension_view.cpp
                            application/editor_application.cpp)

target_include_directories(USKEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(USKEditor EEditorExtension)