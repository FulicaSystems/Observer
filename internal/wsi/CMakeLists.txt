set(component wsi)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    window.hpp
    
    window_glfw.hpp
)

target_sources(${component}
    PRIVATE
    window.hpp

    window_glfw.hpp
    window_glfw.cpp
)

target_link_libraries(${component}
    PUBLIC glfw
    PUBLIC graphics
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")