# glad
add_subdirectory(glad/cmake)
# glad settings for vulkan
glad_add_library(glad_vk_13
	LOADER
	API vulkan=1.3
	EXTENSIONS VK_KHR_get_memory_requirements2,
		VK_KHR_dedicated_allocation,
		VK_KHR_bind_memory2,
		VK_KHR_get_physical_device_properties2,
		VK_KHR_device_group,
		VK_KHR_buffer_device_address,
		VK_EXT_buffer_device_address,
		VK_KHR_surface,
		VK_KHR_swapchain,
		VK_EXT_debug_utils
)
glad_add_library(glad_gl_core_46
	LOADER
	API gl:core=4.6
	EXTENSIONS GL_NV_command_list
)

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
add_subdirectory(vulkanmemoryallocator)