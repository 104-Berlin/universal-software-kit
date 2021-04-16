add_library(EExtension SHARED   extension_manager.cpp
                                ui/engine_ext_ui.cpp)
target_include_directories(EExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


if (WIN32 OR APPLE)
    add_subdirectory(impl)
    target_compile_definitions(EExtension PRIVATE EXT_RENDERER_ENABLED)
    target_link_libraries(EExtension EExtImplRenderer)
endif()

target_compile_definitions(EExtension PRIVATE EXTENSION_EXPORT)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_link_libraries(EExtension dl)
endif()