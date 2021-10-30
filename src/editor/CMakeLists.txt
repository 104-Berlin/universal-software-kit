cmake_minimum_required(VERSION 3.15)
project(USKEditor)

add_executable(USKEditor MACOSX_BUNDLE    editor_entry.cpp
                            ui-fields/editor_ui_fields.cpp
                            ui-fields/editor_basic_object_view.cpp
                            ui-fields/editor_resource_view.cpp
                            application/editor_application.cpp)

target_include_directories(USKEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(USKEditor EEditorExtension)

set(APPS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/USKEditor.app)
set(DIRS ${CMAKE_BINARY_DIR})

install(CODE "include(BundleUtilities)
            fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")



set(CPACK_PACKAGE_FILE_NAME Editor)
set(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns)
set(DYLD_LIBRARY_PATH lib)

set(CPACK_BUNDLE_NAME USKEditor)
set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns)


include(CPack)
