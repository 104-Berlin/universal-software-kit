cmake_minimum_required(VERSION 3.15)
project(USKScripting)

add_subdirectory(lua/lua)

add_library(EScripting SHARED   engine_script_context.cpp
                                engine_scripting_manager.cpp
                                tasks/extension_task.cpp
                                ##chai/engine_chai_context.cpp
                                lua/engine_lua_context.cpp
                                )

target_include_directories(EScripting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(EScripting PRIVATE SCRIPTING_EXPORT)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
target_compile_options(EScripting PRIVATE /bigobj)
endif()

if (UNIX)
target_compile_options(EScripting PRIVATE -fPIC)
endif()


target_link_libraries(EScripting Shared)
target_link_libraries(EScripting Lua)