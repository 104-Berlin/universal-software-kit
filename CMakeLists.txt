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
###### TESTING ##########
if (${BUILD_TEST})
  add_subdirectory(deps/googletets)

  add_executable(UnitTest src/tests/test_engine_file.cpp
                          src/tests/test_engine_events.cpp)
  target_link_libraries(UnitTest gtest_main)
  target_link_libraries(UnitTest Shared)
  if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND USE_COV)
    target_link_libraries(UnitTest --coverage)
  endif()
endif()


#########################
####### EDITOR ##########
if (WIN32 OR APPLE)
  add_subdirectory(deps/usk-graphics)

  add_executable(USKEditor src/editor/editor_entry.cpp)
  target_include_directories(USKEditor PUBLIC src/editor/)
  target_link_libraries(USKEditor GPlatformWrapper)
  target_link_libraries(USKEditor GRenderer)
endif()