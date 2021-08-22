cmake_minimum_required(VERSION 3.15)
project(USKScripting)

add_library(EScripting SHARED   engine_script_context.cpp
                                chai/engine_chai_context.cpp)

target_include_directories(EScripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(EScripting PRIVATE SCRIPTING_EXPORT)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
target_compile_options(EScripting PRIVATE /bigobj)
endif()

target_link_libraries(EScripting Shared)