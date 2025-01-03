set(component internal)

add_library(${component} STATIC "")

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")

target_link_libraries(${component}
    PUBLIC Utilities
)

add_subdirectory(graphics)
add_subdirectory(wsi)
# add_subdirectory(engine_types)
# add_subdirectory(host_memory)

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