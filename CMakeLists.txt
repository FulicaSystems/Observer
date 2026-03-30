# CMake version
cmake_minimum_required(VERSION 3.21)

# C++ standard version
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME Project_O)

# initiating project given its name
project(${PROJECT_NAME})

option(PROJECT_BUILD_TESTS "Build tests" OFF)

include(cmake/dependency.cmake)

add_subdirectory(internal)

if (PROJECT_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(src)
