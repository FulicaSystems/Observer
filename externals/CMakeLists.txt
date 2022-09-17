add_library(glad INTERFACE)
target_sources(glad INTERFACE
	glad/include/glad/gl.h
	glad/include/KHR/khrplatform.h
	glad/src/gl.c
)
target_include_directories(glad INTERFACE glad/include)

add_subdirectory(glfw)