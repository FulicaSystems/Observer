set(component device)

add_library(${component} STATIC "")
add_library(graphics::device ALIAS ${component})

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    physical_device.hpp
    
    device.hpp
)

target_sources(${component}
    PRIVATE
    physical_device.hpp
    physical_device.cpp

    device.hpp
    device.cpp
)

target_link_libraries(${component}
    PUBLIC Vulkan-Headers
    PUBLIC graphics
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")