set(component po_test)

add_executable(${component})

target_sources(${component}
    PRIVATE
    main.cpp
)

target_link_libraries(${component}
    PUBLIC client
    PUBLIC Utilities
)

add_subdirectory(client)