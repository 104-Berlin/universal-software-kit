cmake_minimum_required(VERSION 3.15)
project(USKEditorRendering)

set(USK_IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/imgui)

add_library(USKImgui STATIC ${USK_IMGUI_DIR}/imgui.cpp 
                            ${USK_IMGUI_DIR}/imgui_draw.cpp 
                            ${USK_IMGUI_DIR}/imgui_widgets.cpp
                            ${USK_IMGUI_DIR}/imgui_demo.cpp
                            ${USK_IMGUI_DIR}/imgui_tables.cpp
                            ${USK_IMGUI_DIR}/backends/imgui_impl_glfw.cpp
                            ${USK_IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

target_link_libraries(USKImgui glfw)
target_include_directories(USKImgui PUBLIC ${USK_IMGUI_DIR})

add_library(USKEditorRendering STATIC editor_rendering_entry.cpp)

target_link_libraries(USKEditorRendering USKImgui)