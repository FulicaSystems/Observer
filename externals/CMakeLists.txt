# glfw
add_subdirectory(glfw)

# mathematics
add_subdirectory(mathematics)

# utilities
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