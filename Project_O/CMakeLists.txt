cmake_minimum_required(VERSION 3.21)

project(renderer)

set(CXX_STANDARD 20)

set(HEADER ${PROJECT_SOURCE_DIR}/header)
set(INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(SOURCE ${PROJECT_SOURCE_DIR}/src)

file(GLOB FILES
	${HEADER}/*.h
	${HEADER}/*.hpp
	${INCLUDE}/*.h
	${INCLUDE}/*.hpp
	${SOURCE}/*.c
	${SOURCE}/*.cpp
)

include_directories(renderer ${HEADER} ${INCLUDE})

add_executable(renderer ${FILES})

# external git
include(FetchContent)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.3.6
)
FetchContent_MakeAvailable(glfw)

# external libraries
target_link_libraries(renderer glfw)
target_link_libraries(renderer opengl32)