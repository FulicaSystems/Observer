set(component graphics)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    context.hpp
    instance.hpp
    surface.hpp
    swapchain.hpp
    graphics_api.hpp
)

target_sources(${component}
    PRIVATE
    context.hpp
    context.cpp
    instance.hpp
    instance.cpp
    surface.hpp
    surface.cpp
    swapchain.hpp
    swapchain.cpp
)

target_link_libraries(${component}
    PUBLIC Vulkan-Headers
    PUBLIC Utilities
)

#add_subdirectory(asset)
#add_subdirectory(device)
#add_subdirectory(device_memory)
#add_subdirectory(renderer)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")