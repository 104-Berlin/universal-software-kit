cmake_minimum_required(VERSION 3.15)
project(USKSharedInterface)

add_library(EInterface SHARED   interface_functions.cpp
                                util/interface_util.cpp)

target_include_directories(EInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(EInterface PRIVATE SH_INTERFACE_EXPORT)


target_link_libraries(EInterface EExtension)

if(UNIX)
target_link_libraries(EInterface pthread)
endif(UNIX)
