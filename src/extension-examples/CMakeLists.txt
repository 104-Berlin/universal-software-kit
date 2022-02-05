cmake_minimum_required(VERSION 3.15)
project(USKExamples)

if (${BUILD_EDITOR})
add_library(Example1 MODULE example1.cpp)
set_target_properties(Example1 PROPERTIES SUFFIX ".uex")
set_target_properties(Example1 PROPERTIES PREFIX "")

add_library(TECHDRAW MODULE technical_drawing.cpp)
set_target_properties(TECHDRAW PROPERTIES SUFFIX ".uex")
set_target_properties(TECHDRAW PROPERTIES PREFIX "")


add_library(UISHOW MODULE ui_show.cpp)
set_target_properties(UISHOW PROPERTIES SUFFIX ".uex")
set_target_properties(UISHOW PROPERTIES PREFIX "")

add_library(SHOW_IMAGE MODULE show_image.cpp)
set_target_properties(SHOW_IMAGE PROPERTIES SUFFIX ".uex")
set_target_properties(SHOW_IMAGE PROPERTIES PREFIX "")

if (${BUILD_MIDI_TOOL})
add_library(MIDI_CONTROLL MODULE midi_controll.cpp)
set_target_properties(MIDI_CONTROLL PROPERTIES SUFFIX ".uex")
set_target_properties(MIDI_CONTROLL PROPERTIES PREFIX "")

target_link_libraries(MIDI_CONTROLL EEditorExtension)
target_link_libraries(MIDI_CONTROLL rtmidi)
endif(${BUILD_MIDI_TOOL})

target_link_libraries(Example1 EEditorExtension)
target_link_libraries(TECHDRAW EEditorExtension)
target_link_libraries(UISHOW EEditorExtension)
target_link_libraries(SHOW_IMAGE EEditorExtension)



endif()


add_library(Example2 MODULE example2.cpp)
set_target_properties(Example2 PROPERTIES SUFFIX ".uex")
set_target_properties(Example2 PROPERTIES PREFIX "")

target_link_libraries(Example2 EInterface)