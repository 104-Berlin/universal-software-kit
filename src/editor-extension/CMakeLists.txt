cmake_minimum_required(VERSION 3.15)

project(USKEditorExtension)

file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_SRC *.cpp)
file(GLOB_RECURSE ${CMAKE_PROJECT_NAME}_HDR *.h)

add_library(EEditorExtension SHARED ${${CMAKE_PROJECT_NAME}_SRC}
                                    ${${CMAKE_PROJECT_NAME}_HDR})

target_compile_definitions(EEditorExtension PRIVATE EXPORT_EDITOR_EXTENSION)
target_include_directories(EEditorExtension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(EEditorExtension EInterface)
target_link_libraries(EEditorExtension GRenderer)

if (${BUILD_ASSIMP})

target_compile_definitions(EEditorExtension PUBLIC BUILD_ASSIMP)
target_link_libraries(EEditorExtension assimp)

endif()