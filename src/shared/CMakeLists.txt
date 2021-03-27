cmake_minimum_required(VERSION 3.15)
project(USKShared)

add_library(Shared SHARED   file/engine_file_collection.cpp
                            file/engine_file.cpp
                            file/engine_folder.cpp
                            file/engine_shared_buffer.cpp
                            logging/engine_logging.cpp)
target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(Shared PRIVATE EEXPORT)


if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
target_link_libraries(Shared --coverage)
endif()
