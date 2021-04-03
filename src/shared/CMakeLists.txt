cmake_minimum_required(VERSION 3.15)
project(USKShared)

add_subdirectory(glm)


add_library(Shared SHARED   file/engine_file_collection.cpp
                            file/engine_file.cpp
                            file/engine_folder.cpp
                            file/engine_shared_buffer.cpp
                            register/engine_data_handle.cpp
                            event/engine_event_dispatcher.cpp
                            logging/engine_logging.cpp)

target_link_libraries(Shared glm)
target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(Shared PRIVATE EEXPORT)
