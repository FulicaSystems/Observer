cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "updating submodules")
execute_process(COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_RESULT
)
if(NOT GIT_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to update submodules, ${GIT_RESULT}")
endif()

project(Project_O)

add_subdirectory(externals)

set(HEADERS
    include/application.hpp
    include/lowrenderer.hpp
    include/graphicsdevice.hpp
    include/graphicspipeline.hpp
)

set(SOURCES
    src/main.cpp

    src/application.cpp
    src/lowrenderer.cpp
    src/graphicsdevice.cpp
    src/graphicspipeline.cpp
)

add_executable(renderer ${HEADERS} ${SOURCES})
target_include_directories(renderer PRIVATE include)
target_link_libraries(renderer
    PUBLIC glad
    PUBLIC glfw
    PUBLIC libraries
)