# glad
add_subdirectory(glad/cmake)
# glad settings for vulkan
glad_add_library(glad_vk_13
	LOADER
	API vulkan=1.3
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