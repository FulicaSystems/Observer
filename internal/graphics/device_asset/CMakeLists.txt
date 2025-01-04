set(component device_asset)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    pipeline.hpp
    shader.hpp
    shadermodule.hpp
)

target_sources(${component}
    PRIVATE
    shader.cpp
    shader.hpp
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")