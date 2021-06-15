cmake_minimum_required(VERSION 3.15)
project(USKTests)

add_executable(UnitTest test_engine_file.cpp
                        test_engine_register.cpp
                        test_engine_extension.cpp
                        test_engine_resource.cpp
                        test_engine_events.cpp)


target_link_libraries(UnitTest gtest_main)
target_link_libraries(UnitTest EExtension)