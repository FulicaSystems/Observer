# glad
add_library(glad INTERFACE)
target_sources(glad INTERFACE
	#vulkan
	glad/include/vk_platform.h
	glad/include/glad/vulkan.h
	glad/src/vulkan.c
	#opengl
	glad/include/glad/gl.h
	glad/include/KHR/khrplatform.h
	glad/src/gl.c
)
target_include_directories(glad INTERFACE glad/include)

# glfw
add_subdirectory(glfw)

# libraries
add_subdirectory(libraries)

# glslc
add_subdirectory(glslang)

# spirv
set(SPIRV_HEADERS_SKIP_EXAMPLES ON)
set(SPIRV_HEADERS_SKIP_INSTALL ON)
add_subdirectory(spirv-headers)
add_subdirectory(spirv-tools)

# shaderc
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_TESTS ON)
add_subdirectory(shaderc)

# vulkan memory allocator
set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
set(VMA_DYNAMIC_VULKAN_FUNCTIONS ON)
add_subdirectory(vulkanmemoryallocator)