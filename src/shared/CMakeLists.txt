cmake_minimum_required(VERSION 3.15)
project(USKShared)


add_library(Shared SHARED   file/engine_file_collection.cpp
                            file/engine_file.cpp
                            file/engine_folder.cpp
                            file/engine_shared_buffer.cpp
                            event/engine_event_dispatcher.cpp
                            storage/engine_storage_description.cpp
                            storage/engine_storage_values.cpp
                            register/engine_register.cpp
                            register/engine_serializer.cpp
                            register/engine_deserializer.cpp
                            resource/engine_resource_manager.cpp
                            resource/engine_resource_storage_link.cpp
                            logging/engine_logging.cpp)

target_include_directories(Shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(Shared PRIVATE SHARED_EXPORT)
