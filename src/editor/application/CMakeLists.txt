cmake_minimum_required(VERSION 3.15)

add_executable(EditorApp entry.cpp)

target_link_libraries(EditorApp SoftwareGui glfw)