cmake_minimum_required(VERSION 3.15)
project(USK)

# GLOBAL SETTINGS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


option(USE_COV "Use the covarage option when not using MSVC" FALSE)
option(BUILD_EDITOR "Build the editor for mac or windows" TRUE)
option(BUILD_EXAMPLES "Build exmaples" TRUE)
option(BUILD_TEST "Toggle the unittest" TRUE)
option(BUILD_CLT "Toggle the Command-Line-Tool" TRUE)
option(BUILD_TOOLS "Toogle the build of the tools" TRUE)
option(BUILD_MIDI_TOOL "Toggle the midi extension" TRUE)
option(BUILD_ASSIMP "Toggle the assimp build" TRUE)


# Editor only for windows and mac supported
if (NOT WIN32 AND NOT APPLE)
set(BUILD_EDITOR FALSE)
endif()

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND USE_COV)
add_compile_options(--coverage)
link_libraries(--coverage)
endif()


# Add system definitions
if (${BUILD_TEST})
add_compile_definitions(E_TEST)
endif()
if (WIN32)
add_compile_definitions(EWIN)
elseif (APPLE)
add_compile_definitions(EMAC)
else()
add_compile_definitions(EUNI)
endif(WIN32)

include_directories(src/)

add_subdirectory(src/util)
add_subdirectory(src/shared)
add_subdirectory(src/extension)
add_subdirectory(src/scripting)

add_subdirectory(src/interface)

#########################
######## ASSIMP #########
if (${BUILD_ASSIMP})
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
add_subdirectory(deps/assimp)
endif()

#########################
######### TOOLS #########
if (${BUILD_TOOLS})
add_subdirectory(src/tools)
endif()

#########################
######### MIDI ##########
if (${BUILD_MIDI_TOOL} AND ${BUILD_EDITOR})
add_subdirectory(deps/rtmidi)
endif()

#########################
##### COMMAND_LINE ######

if (${BUILD_CLT})
  add_subdirectory(src/command-line)
endif()

#########################
###### TESTING ##########
if (${BUILD_TEST})
  add_subdirectory(deps/googletets)
  add_subdirectory(src/tests)
endif()




#########################
######## EXAMPLES #######
if (${BUILD_EXAMPLES})
add_subdirectory(src/extension-examples)
endif()



#########################
####### EDITOR ##########
if ((WIN32 OR APPLE) AND ${BUILD_EDITOR})
set(BUILD_SHARED_LIBS FALSE)
  add_subdirectory(src/editor-extension)
  add_subdirectory(deps/usk-graphics)

  add_subdirectory(src/editor)
endif()
