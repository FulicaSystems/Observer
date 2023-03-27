# **Project O**

Vulkan renderer, based on [this repository](https://github.com/sjarod/vulkan-tutorial).

## Summary
- [Getting started](#getting-started)
    - [Installation](#installation)
    - [Shaders](#shaders)
- [Third-parties](#third-parties)

# Getting started

## Installation
This is a CMake-based project so make sure to install CMake ([portable version](https://github.com/Kitware/CMake/releases/download/v3.26.0-rc5/cmake-3.26.0-rc5-windows-x86_64.zip) : do not forget to add bin directory to PATH).

glad files are generated using Python and the Jinja2 package, [install Python](https://www.python.org/ftp/python/3.11.2/python-3.11.2-amd64.exe) and run the command :
```
pip install -U Jinja2
```
Python is also used to compile shaderc.

Download the [latest Vulkan SDK](https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe) from [LunarG's website](https://vulkan.lunarg.com/sdk/home#), it is used to make the Vulkan validations layers available.

## Shaders
Compile the shaders running the command :
```
glslc shader.vert -o shader.vert.spv
glslc shader.frag -o shader.frag.spv
```

# Third-parties
- glad 2
    - https://github.com/Dav1dde/glad
    - https://gen.glad.sh/
- GLFW
    - https://www.glfw.org/
- Python
    - https://www.python.org/
- CMake
    - https://cmake.org/
- Vulkan SDK
    - https://vulkan.lunarg.com/
- shaderc
    - https://github.com/google/shaderc
