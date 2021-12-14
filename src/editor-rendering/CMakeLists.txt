cmake_minimum_required(VERSION 3.15)
project(USKEditorRendering)

set(USK_IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/imgui)
set(USK_DILIGENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/diligent-core)

add_library(USKImgui STATIC ${USK_IMGUI_DIR}/imgui.cpp 
                            ${USK_IMGUI_DIR}/imgui_draw.cpp 
                            ${USK_IMGUI_DIR}/imgui_widgets.cpp
                            ${USK_IMGUI_DIR}/imgui_demo.cpp
                            ${USK_IMGUI_DIR}/imgui_tables.cpp
                            ${USK_IMGUI_DIR}/backends/imgui_impl_glfw.cpp
                            ${USK_IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

target_link_libraries(USKImgui glfw)
target_include_directories(USKImgui PUBLIC  ${USK_IMGUI_DIR}
                                            ${CMAKE_CURRENT_SOURCE_DIR})

add_library(USKEditorRendering SHARED   editor_rendering_entry.cpp
                                        ${USK_IMGUI_DIR}/backends/imgui_impl_glfw.cpp
                                        ${USK_IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

target_compile_definitions(USKEditorRendering PRIVATE EXPORT_EDITOR_RENDERING)


target_link_libraries(USKEditorRendering    Diligent-Common
                                            Diligent-GraphicsTools
                                            Diligent-GraphicsEngineOpenGL-shared
                                            glfw
                                            ImGui)

target_include_directories(USKEditorRendering PUBLIC ${USK_DILIGENT_DIR}
                                                     ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(USKEditorRendering PRIVATE ENGINE_DLL=1)


if(WIN32)
copy_required_dlls(USKEditorRendering)
endif()
