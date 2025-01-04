#include <cassert>

#include <graphics/device/device.hpp>
#include <graphics/device/physical_device.hpp>
#include <graphics/context.hpp>
#include <wsi/window_glfw.hpp>

#include "application.hpp"

Application::Application()
{
    wsi = std::make_unique<WSILoaderGLFW>();
    wsi->init();

    window = std::make_unique<WindowGLFW>(1366, 768);

    context =
        std::make_unique<Context>("Renderer", VERSION(0, 0, 0), VERSION(0, 0, 0), window->getRequiredExtensions());

    for (const std::string& physicalDeviceName : context->enumerateAvailablePhysicalDevices(false))
    {
        std::shared_ptr<PhysicalDevice> physicalDevice = std::make_shared<PhysicalDevice>(*context, physicalDeviceName.c_str());
        assert(physicalDevice);

        physicalDevices.push_back(physicalDevice);
        devices.emplace_back(physicalDevice->createDevice());
    }
}

Application::~Application()
{
    devices.clear();
    physicalDevices.clear();
    context.reset();
    
    window.reset();
    wsi.reset();
}

void Application::run()
{
    while (!window->shouldClose())
    {
        window->swapBuffers();
        window->pollEvents();
        // TODO : threadpool poll main queue
    }

    wsi->terminate();
}