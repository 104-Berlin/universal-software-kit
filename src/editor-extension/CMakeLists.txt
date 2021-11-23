cmake_minimum_required(VERSION 3.15)

project(USKEditorExtension)

add_library(EEditorExtension SHARED editor_vector_functions.cpp        
                                    default-types/editor_def_res_image.cpp
                                    ui/editor_ext_viewport.cpp
                                    ui/editor_ext_viewport_tools.cpp
                                    ui/editor_ext_ui.cpp)

target_compile_definitions(EEditorExtension PRIVATE EXPORT_EDITOR_EXTENSION)
target_include_directories(EEditorExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(BGFX_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../../deps/bgfx)
set(BGFX_LIB_DIR ${BGFX_BASE_DIR}/win/${CMAKE_BUILD_TYPE})

target_link_libraries(EEditorExtension EInterface)
target_link_libraries(EEditorExtension ${BGFX_LIB_DIR}/bgfx.lib)
target_link_libraries(EEditorExtension ${BGFX_LIB_DIR}/bimg.lib)
target_link_libraries(EEditorExtension ${BGFX_LIB_DIR}/bimg_decoder.lib)
target_link_libraries(EEditorExtension ${BGFX_LIB_DIR}/bx.lib)
target_include_directories(EEditorExtension PUBLIC ${BGFX_BASE_DIR}/include)
target_include_directories(EEditorExtension PUBLIC ${BGFX_BASE_DIR}/include/compat/msvc)
target_compile_definitions(EEditorExtension PUBLIC BX_CONFIG_DEBUG=0)