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
    include/format.hpp
    include/application.hpp

    include/graphicsobject.hpp
    include/graphicsdevice.hpp
    include/graphicspipeline.hpp

    include/vertex.hpp
    include/vertexbuffer.hpp

    include/commandbuffer.hpp
    include/commandpool.hpp

    include/memoryallocator.hpp

    include/lowrenderer.hpp
    include/renderer.hpp
)

set(SOURCES
    src/main.cpp

    src/format.cpp
    src/application.cpp

    src/graphicsdevice.cpp
    src/graphicspipeline.cpp

    src/vertex.cpp

    src/commandbuffer.cpp
    src/commandpool.cpp

    src/memoryallocator.cpp

    src/lowrenderer.cpp
    src/renderer.cpp
)

add_executable(renderer ${HEADERS} ${SOURCES})
target_include_directories(renderer PRIVATE include)
target_link_libraries(renderer
    PUBLIC glad
    PUBLIC glfw
    PUBLIC libraries
)