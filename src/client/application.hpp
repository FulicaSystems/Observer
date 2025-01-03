#pragma once

#include <memory>
#include <vector>

class WSILoaderI;
class WindowGLFW;
class Context;
class PhysicalDevice;
class LogicalDevice;

class Application
{
  private:
    std::unique_ptr<WSILoaderI> wsi;
    std::unique_ptr<WindowGLFW> window;

    std::unique_ptr<Context> context;

    std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices;

    std::vector<std::shared_ptr<LogicalDevice>> devices;

  public:
    Application();

    void run();
};