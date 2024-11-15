set(component internal)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    mesh.hpp
    resource.hpp
    resource_loader.hpp
    scene.hpp
    vertex.hpp
)

target_sources(${component}
    PRIVATE
    mesh.cpp
    mesh.hpp
    resource_loader.cpp
    resource_loader.hpp
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")

add_subdirectory(graphics)
add_subdirectory(wsi)

# installing build (using cmake --install command)
install(TARGETS ${component})
# installing the include files
install(
    DIRECTORY internal/
    DESTINATION include
    FILES_MATCHING
    PATTERN *.h
    PATTERN *.hpp
    PATTERN *.inl
)