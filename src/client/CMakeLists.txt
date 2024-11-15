set(component client)

add_library(${component} STATIC "")

target_sources(${component}
    PRIVATE
    application.cpp
    application.hpp
)

target_link_libraries(${component}
    PUBLIC wsi
    PUBLIC graphics
)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")