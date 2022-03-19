cmake_minimum_required(VERSION 3.21)

project(Project_O)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# external git
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 7d5a16c # 3.3.6
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
	libraries
	GIT_REPOSITORY https://github.com/SJarod/libraries.git
	GIT_TAG master
)
FetchContent_MakeAvailable(libraries)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG c71a50d # v1.87
)
FetchContent_MakeAvailable(imgui)
add_library(imgui INTERFACE)
target_include_directories(imgui
	INTERFACE ${imgui_SOURCE_DIR}
	INTERFACE ${imgui_SOURCE_DIR}/backends
)

file(GLOB IMGUI_SOURCES
	${imgui_SOURCE_DIR}/*.h
	${imgui_SOURCE_DIR}/*.hpp
	${imgui_SOURCE_DIR}/*.c
	${imgui_SOURCE_DIR}/*.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3_loader.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

set(HEADER ${PROJECT_SOURCE_DIR}/header)
set(INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(SOURCE ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES
	${HEADER}/*.h
	${HEADER}/*.hpp
	${HEADER}/*.inl
	${INCLUDE}/*.h
	${INCLUDE}/*.hpp
	${INCLUDE}/*.inl
	${SOURCE}/*.c
	${SOURCE}/*.cpp
)

add_executable(renderer ${IMGUI_SOURCES} ${SOURCES})
target_include_directories(renderer
	PRIVATE ${HEADER}
	PRIVATE ${INCLUDE}
)

# external libraries
target_link_libraries(renderer glfw)
target_link_libraries(renderer opengl32)
target_link_libraries(renderer libraries)
target_link_libraries(renderer imgui)