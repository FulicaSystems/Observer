set(component data)

add_library(${component} STATIC "")

# public headers will be installed with the target
set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    # add public headers here
    # ...
    resource_manager.hpp
    resource.hpp
)

target_sources(${component}
    PRIVATE
    # add sources and private headers here
    # ...
    resource_manager.hpp
    resource_manager.cpp

    resource.hpp
)


target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")
