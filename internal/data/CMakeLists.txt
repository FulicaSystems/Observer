set(component data)

add_library(${component} STATIC "")

# public headers will be installed with the target
set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    # add public headers here
    # ...
    resource_manager.hpp
    resource.hpp

    saved/mesh.hpp
    saved/scene.hpp
)

target_sources(${component}
    PRIVATE
    # add sources and private headers here
    # ...
    resource_manager.hpp
    resource_manager.cpp

    resource.hpp

    saved/mesh.hpp
    saved/mesh.cpp

    saved/scene.hpp
    saved/scene.cpp
)

# TODO : use fulica's mathematics library
depends(PACKAGE FROM_SOURCE DIRECTORY glm/install CONFIG glm)
target_link_libraries(${component}
    PUBLIC glm::glm
    PUBLIC graphics::device
    PUBLIC renderer
)

target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")
