add_library(glad INTERFACE)
target_sources(glad INTERFACE
	glad/include/vk_platform.h
	glad/include/glad/vulkan.h
	glad/src/vulkan.c

	glad/include/glad/gl.h
	glad/include/KHR/khrplatform.h
	glad/src/gl.c
)
target_include_directories(glad INTERFACE glad/include)

add_subdirectory(glfw)

add_subdirectory(libraries)

#glslc
add_subdirectory(glslang)

set(SPIRV_HEADERS_SKIP_EXAMPLES ON)
set(SPIRV_HEADERS_SKIP_INSTALL ON)
add_subdirectory(spirv-headers)
add_subdirectory(spirv-tools)

set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_TESTS ON)
add_subdirectory(shaderc)