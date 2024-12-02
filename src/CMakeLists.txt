set(component po_test)

add_executable(${component})

target_sources(${component}
    PRIVATE
    main.cpp
)

add_subdirectory(client)

target_link_libraries(${component}
    PUBLIC client
)