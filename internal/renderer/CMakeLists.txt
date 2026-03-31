set(component renderer)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    renderer.hpp
)

#target_sources(${component}
#    PRIVATE
#    renderer.cpp
#    renderer.hpp
#)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")