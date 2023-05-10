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

    include/memorymanager.hpp
    include/allocator.hpp
    include/vmahelper.hpp

    include/lowrenderer.hpp
    include/renderer.hpp

    include/resource.hpp
    include/resourcesmanager.hpp
    include/mesh.hpp
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

    src/allocator.cpp
    src/vmahelper.cpp

    src/lowrenderer.cpp
    src/renderer.cpp

    src/mesh.cpp
)

add_executable(renderer ${HEADERS} ${SOURCES})
target_include_directories(renderer PRIVATE include)
target_link_libraries(renderer
    PUBLIC glad_vk_13
    #PUBLIC glad_gl_core_46
    PUBLIC glfw
    PUBLIC libraries
    PUBLIC VulkanMemoryAllocator
)