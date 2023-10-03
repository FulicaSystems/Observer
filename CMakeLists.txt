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
    include/context.hpp
    include/datatypes.hpp
    include/device.hpp
    include/format.hpp
    include/graphicsapi.hpp
    include/memoryallocator.hpp
    include/mesh.hpp
    include/pipeline.hpp
    include/queue.hpp
    include/renderer.hpp
    include/resource.hpp
    include/resourcesmanager.hpp
    include/scene.hpp
    include/shader.hpp
    include/vertex.hpp
    include/vmahelper.hpp
    include/window.hpp
)

set(SOURCES
    src/application.cpp
    src/format.cpp
    src/lowrenderer.cpp
    src/lowrenderer_gl.cpp
    src/lowrenderer_vk.cpp
    src/main.cpp
    src/memoryallocator.cpp
    src/mesh.cpp
    src/resourcesmanager.cpp
    src/shader.cpp
    src/shadermodule.cpp
    src/shadermodule_vk.cpp
    src/vertex.cpp
    src/vertexbuffer.cpp
    src/vmahelper.cpp
)

add_executable(renderer ${HEADERS} ${SOURCES})
target_include_directories(renderer PRIVATE include)
target_link_libraries(renderer
    PUBLIC glad_vk_13
    PUBLIC glad_gl_core_46
    PUBLIC glfw
    PUBLIC libraries
    PUBLIC VulkanMemoryAllocator
)
