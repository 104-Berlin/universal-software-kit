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
option(BUILD_EXAMPLES "Build exmaples" TRUE)

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
add_subdirectory(src/extension)


###############################
######### DEPENDENCIES ########

target_link_libraries(EExtension Shared)



#########################
###### TESTING ##########
if (${BUILD_TEST})
  add_subdirectory(deps/googletets)

  add_executable(UnitTest src/tests/test_engine_file.cpp
                          src/tests/test_engine_register.cpp
                          src/tests/test_engine_extension.cpp
                          src/tests/test_engine_resource.cpp
                          src/tests/test_engine_events.cpp)
  target_link_libraries(UnitTest gtest_main)
  target_link_libraries(UnitTest EExtension)
  if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND USE_COV)
    target_link_libraries(UnitTest --coverage)
  endif()
endif()



#########################
######## EXAMPLES #######
if (${BUILD_EXAMPLES})
add_subdirectory(src/extension-examples)

target_link_libraries(Example2 EExtension)
endif()



#########################
####### EDITOR ##########
if ((WIN32 OR APPLE) AND ${BUILD_EDITOR})
  add_subdirectory(src/editor-extension)
  add_subdirectory(deps/usk-graphics)

  add_executable(USKEditor  src/editor/editor_entry.cpp
                            src/editor/ui-fields/editor_ui_fields.cpp
                            src/editor/application/editor_application.cpp)
  target_include_directories(USKEditor PUBLIC src/editor/)
  target_link_libraries(EEditorExtension EExtension)
  target_link_libraries(EEditorExtension GRenderer)

  target_link_libraries(USKEditor EEditorExtension)
  if (${BUILD_EXAMPLES})
    target_link_libraries(Example1 EEditorExtension)
  endif()
endif()

