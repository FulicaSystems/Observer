# glad
add_subdirectory(glad/cmake)
# glad settings for vulkan
glad_add_library(glad_vk_13
	API vulkan=1.3
	EXTENSIONS VK_EXT_debug_utils,VK_KHR_surface,VK_KHR_swapchain
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
set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
set(VMA_DYNAMIC_VULKAN_FUNCTIONS ON)
add_subdirectory(vulkanmemoryallocator)