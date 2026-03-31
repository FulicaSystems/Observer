set(component device)

option(OBSERVER_OPTION_ENABLE_VIDEO_TRANSCODE "enable video transcoding" OFF)

if (OBSERVER_OPTION_ENABLE_VIDEO_TRANSCODE)
    add_compile_definitions(ENABLE_VIDEO_TRANSCODE)
endif()


add_library(${component} STATIC "")
add_library(graphics::device ALIAS ${component})

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    physical_device.hpp
    
    device.hpp

    memory/buffer.hpp
    memory/image.hpp

    asset/render_pass.hpp
)

target_sources(${component}
    PRIVATE
    physical_device.hpp
    physical_device.cpp

    device.hpp
    device.cpp

    memory/buffer.hpp
    memory/image.hpp

    asset/render_pass.hpp
)

depends(MODULE CONFIG Vulkan)
depends(PRECOMPILED CONFIGURE_DEPENDS OUTPUT_TARGET F6::Bin DIRECTORIES project_binary/install FILES binary INCLUDE_PATHS include)

target_include_directories(${component}
    PUBLIC ${Vulkan_INCLUDE_DIR}
)
target_link_libraries(${component}
    PUBLIC ${Vulkan_LIBRARY}
    PUBLIC F6::Bin
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")