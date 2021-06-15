cmake_minimum_required(VERSION 3.15)
project(USKExamples)

if (${BUILD_EDITOR})
add_library(Example1 MODULE example1.cpp)
set_target_properties(Example1 PROPERTIES SUFFIX ".uex")
set_target_properties(Example1 PROPERTIES PREFIX "")

target_link_libraries(Example1 EEditorExtension)
endif()


add_library(Example2 MODULE example2.cpp)
set_target_properties(Example2 PROPERTIES SUFFIX ".uex")
set_target_properties(Example2 PROPERTIES PREFIX "")

target_link_libraries(Example2 EExtension)