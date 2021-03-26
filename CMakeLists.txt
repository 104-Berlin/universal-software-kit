cmake_minimum_required(VERSION 3.15)
project(USK)

# GLOBAL SETTINGS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Add system definitions
if (WIN32)
add_compile_definitions(EWIN)
else()
add_compile_definitions(EUNI)
endif(WIN32)

include_directories(src/)

add_executable(USK_StartTesting src/test_executable.cpp)

add_subdirectory(src/shared)
target_link_libraries(USK_StartTesting Shared)
target_compile_definitions(USK_StartTesting PRIVATE EEXPORT)

#add_subdirectory(src/ecs)
#target_link_libraries(EntityModel)



#########################
###### TESTING ##########
add_subdirectory(deps/googletets)

add_executable(UnitTest src/tests/test_engine_file.cpp)
target_link_libraries(UnitTest gtest_main)
target_link_libraries(UnitTest Shared)