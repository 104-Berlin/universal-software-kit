add_library(EExtension SHARED   extension_manager.cpp)
target_include_directories(EExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(EExtension PRIVATE EXTENSION_EXPORT)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_link_libraries(EExtension dl)
endif()