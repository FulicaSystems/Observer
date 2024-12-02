#pragma once

#include <memory>
#include <vector>

#include "wsi/window_glfw.hpp"

#include "graphics/context.hpp"

#include "graphics/device/physical_device.hpp"

class Application
{
  private:
    std::unique_ptr<WSILoaderI> wsi;
    std::unique_ptr<WindowGLFW> window;

    std::unique_ptr<Context> context;

    std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices;

  public:
    Application();

    void run();
};