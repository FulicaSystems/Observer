set(component wsi)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    window.hpp
)

target_sources(${component}
    PRIVATE
    window.cpp
    window.hpp
)

target_link_libraries(${component}
    PUBLIC glfw
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")