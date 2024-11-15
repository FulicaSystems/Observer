# glfw
add_subdirectory(glfw)

# vulkan headers
add_subdirectory(Vulkan-Headers)

# mathematics
add_subdirectory(mathematics)

# utilities
set(BUILD_AI_UTILS OFF)
add_subdirectory(utilities)

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