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

#add_subdirectory(src/ecs)
#target_link_libraries(EntityModel)