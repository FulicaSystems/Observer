set(component renderer)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    renderer.hpp

    render_state.hpp
)

target_sources(${component}
   PRIVATE
   renderer.cpp
   renderer.hpp

   render_state.hpp
)

depends(MODULE CONFIG Vulkan)

target_include_directories(${component}
    PUBLIC ${Vulkan_INCLUDE_DIR}
)
target_link_libraries(${component}
    PUBLIC ${Vulkan_LIBRARY}
    PUBLIC graphics
    PUBLIC data
)


target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")