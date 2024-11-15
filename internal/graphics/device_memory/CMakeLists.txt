set(component device_memory)

add_library(${component} STATIC "")

set_property(TARGET ${component}
    PROPERTY PUBLIC_HEADER
    buffer.hpp
    image.hpp
    memoryallocator.hpp
    vmahelper.hpp
)

#target_sources(${component}
#    PRIVATE
#    class2.cpp
#    class2.hpp
#)

target_include_directories(${component} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${component} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/..")