#pragma once

#include <memory>
#include <vector>

class WSILoaderI;
class WindowGLFW;
class Context;
class Instance;
class PhysicalDevice;
class LogicalDevice;

class Application
{
  private:
    uint32_t width = 1366U, height = 768U;
    std::unique_ptr<WSILoaderI> m_wsi;
    std::unique_ptr<WindowGLFW> m_window;

    std::unique_ptr<Context> m_context;
    std::unique_ptr<Instance> m_instance;

    std::vector<std::shared_ptr<PhysicalDevice>> m_physicalDevices;
    std::vector<std::unique_ptr<LogicalDevice>> m_devices;

    /**
     * @brief used to select the wanted logical device (defaults to first discrete device if available)
     *
     */
    int m_currentDeviceIndex = -1;

  public:
    Application();
    ~Application();

    int perFrame();
};