set(component graphics)

add_library(${component} STATIC "")

#add_subdirectory(device_asset)
add_subdirectory(device)
#add_subdirectory(device_memory)
#add_subdirectory(renderer)

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
    symbols.hpp
    symbols.cpp
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
    PUBLIC graphics::device

    PUBLIC FuUtilities
    PUBLIC FuMathematics
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")