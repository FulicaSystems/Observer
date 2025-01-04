set(component device)

option(OPTION_ENABLE_VIDEO_TRANSCODE "enable video transcoding" OFF)

if (OPTION_ENABLE_VIDEO_TRANSCODE)
    add_compile_definitions(ENABLE_VIDEO_TRANSCODE)
endif()


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
    PUBLIC Utilities
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")