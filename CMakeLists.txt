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
    include/commandbuffer_vk.hpp
    include/format.hpp
    include/graphicsapi.hpp
    include/lowrenderer.hpp
    include/lowrenderer_gl.hpp
    include/lowrenderer_vk.hpp
    include/memoryallocator.hpp
    include/mesh.hpp
    include/resource.hpp
    include/resourcesmanager.hpp
    include/scene.hpp
    include/shader.hpp
    include/shadermodule.hpp
    include/shadermodule_vk.hpp
    include/vertex.hpp
    include/vertexbuffer.hpp
    include/vertexbuffer_gl.hpp
    include/vertexbuffer_vk.hpp
    include/vmahelper.hpp
)

set(SOURCES
    src/application.cpp
    src/commandbuffer_vk.cpp
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
