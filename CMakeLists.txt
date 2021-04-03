cmake_minimum_required(VERSION 3.15)
project(USK)

# GLOBAL SETTINGS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

option(USE_COV "Use the covarage option when not using MSVC" FALSE)
option(BUILD_EDITOR "Build the editor for mac or windows" TRUE)

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND USE_COV)
add_compile_options(--coverage)
link_libraries(--coverage)
endif()

# set global define unittesting enabled
option(BUILD_TEST "Toggle the unittest" TRUE)
if (${BUILD_TEST})
add_compile_definitions(E_TEST)
endif()


# Add system definitions
if (WIN32)
add_compile_definitions(EWIN)
else()
add_compile_definitions(EUNI)
endif(WIN32)

include_directories(src/)
add_subdirectory(src/shared)


#########################
######## IMGUI ##########

if (${BUILD_EDITOR})
  if (WIN32 OR APPLE)
    add_library(ImGui STATIC  deps/imgui/imgui_demo.cpp
                              deps/imgui/imgui_draw.cpp
                              deps/imgui/imgui_widgets.cpp
                              deps/imgui/imgui.cpp
                              deps/imgui/imgui_tables.cpp)

    target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/
                                            ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/backends/)

    if (WIN32)
    elseif(APPLE)

      find_library(OpenGL_LIBRARY OpenGL)
      find_library(COCOA_LIBRARY Cocoa)
      find_library(METAL_LIBRARY Metal)
      find_library(METALKIT_LIBRARY MetalKit)
      find_library(APPKIT_LIBRARY AppKit)
      find_library(CORE_LIBRARY QuartzCore)

      target_compile_options(ImGui PUBLIC -fobjc-arc)

      target_link_libraries(ImGui ${OpenGL_LIBRARY})
      target_link_libraries(ImGui ${COCOA_LIBRARY})
      target_link_libraries(ImGui ${METAL_LIBRARY})
      target_link_libraries(ImGui ${METALKIT_LIBRARY})
      target_link_libraries(ImGui ${CORE_LIBRARY})
      target_link_libraries(ImGui ${APPKIT_LIBRARY})
    endif(WIN32)
  endif()
endif()



#########################
###### TESTING ##########
if (${BUILD_TEST})
  add_subdirectory(deps/googletets)

  add_executable(UnitTest src/tests/test_engine_file.cpp
                          src/tests/test_engine_register.cpp
                          src/tests/test_engine_events.cpp)
  target_link_libraries(UnitTest gtest_main)
  target_link_libraries(UnitTest Shared)
  if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND USE_COV)
    target_link_libraries(UnitTest --coverage)
  endif()
endif()

#########################
####### EDITOR ##########
if (${BUILD_EDITOR})
  if (WIN32 OR APPLE)
    if (WIN32)
      set(EDITOR_EXTRA_SOURCES src/editor/platform/win32/editor_platform_win32.cpp)
    elseif(APPLE)
      set(EDITOR_EXTRA_SOURCES src/editor/platform/macos/editor_platform_win32.cpp)
    endif()

    add_executable(USKEditor  src/editor/editor_main.cpp
                              src/editor/editor_application.cpp
                              src/editor/graphics/editor_buffer.cpp
                              src/editor/graphics/editor_camera.cpp
                              src/editor/graphics/editor_render_command_queue.cpp
                              src/editor/graphics/editor_render_context.cpp
                              src/editor/graphics/editor_renderer.cpp
                              src/editor/graphics/editor_shader.cpp
                              src/editor/graphics/editor_texture.cpp
                              src/editor/graphics/editor_ui_renderer.cpp
                              src/editor/platform/opengl/editor_opengl_buffer.cpp
                              src/editor/platform/opengl/editor_opengl_render_context.cpp
                              src/editor/platform/opengl/editor_opengl_shader.cpp
                              src/editor/platform/opengl/editor_opengl_texture.cpp
                              ${EDITOR_EXTRA_SOURCES}
                              deps/imgui/backends/imgui_impl_glfw.cpp
                              deps/glad/src/glad.c
                              deps/imgui/backends/imgui_impl_opengl3.cpp)

    target_link_libraries(USKEditor Shared)
    target_link_libraries(USKEditor ImGui)
    target_compile_definitions(USKEditor PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

    add_subdirectory(deps/glfw)
    target_link_libraries(USKEditor glfw)

    target_include_directories(USKEditor PRIVATE src/editor/
                                                  deps/glad/include
                                                  deps/glfw/include)
  endif()
endif()