#include <wsi/window_glfw.hpp>
#include <graphics/context.hpp>
#include <graphics/device/physical_device.hpp>
#include <graphics/device/device.hpp>

#include "application.hpp"

Application::Application()
{
    wsi = std::make_unique<WSILoaderGLFW>();
    wsi->init();

    window = std::make_unique<WindowGLFW>(1366, 768);

    context =
        std::make_unique<Context>("Renderer", VERSION(0, 0, 0), VERSION(0, 0, 0), window->getRequiredExtensions());

    for (const char *physicalDeviceName : context->enumerateAvailablePhysicalDevices(false))
    {
        physicalDevices.emplace_back(std::make_shared<PhysicalDevice>(*context, physicalDeviceName));
        devices.emplace_back((*physicalDevices.end())->createDevice());
    }

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