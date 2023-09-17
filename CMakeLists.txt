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
)

set(SOURCES
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
