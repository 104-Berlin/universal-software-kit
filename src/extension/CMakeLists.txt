add_library(EExtension SHARED ui/engine_ext_ui.cpp)
target_include_directories(EExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(EExtension PRIVATE EXTENSION_EXPORT)