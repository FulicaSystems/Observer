cmake_minimum_required(VERSION 3.21)

project(Project_O)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(HEADER ${PROJECT_SOURCE_DIR}/header)
set(INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(SOURCE ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES
	${HEADER}/*.h
	${HEADER}/*.hpp
	${INCLUDE}/*.h
	${INCLUDE}/*.hpp
	${SOURCE}/*.c
	${SOURCE}/*.cpp
)

add_executable(renderer ${SOURCES})
target_include_directories(renderer
	PRIVATE ${HEADER}
	PRIVATE ${INCLUDE}
)

# external git
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.3.6
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
	libraries
	GIT_REPOSITORY https://github.com/SJarod/libraries.git
	GIT_TAG master
)
FetchContent_MakeAvailable(libraries)

# external libraries
target_link_libraries(renderer glfw)
target_link_libraries(renderer opengl32)
target_link_libraries(renderer libraries)