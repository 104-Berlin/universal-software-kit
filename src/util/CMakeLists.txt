cmake_minimum_required(VERSION 3.15)
project(USKUtil)


add_library(Util STATIC     file/engine_file_collection.cpp
                            file/engine_file.cpp
                            file/engine_folder.cpp
                            file/engine_shared_buffer.cpp
                            file/engine_base64.cpp
                            string/util_string.cpp
                            logging/engine_logging.cpp)

target_include_directories(Util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(Util PRIVATE SHARED_EXPORT)




if (UNIX)
target_compile_options(Util PRIVATE -fPIC)
endif()
