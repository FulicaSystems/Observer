#include "application.hpp"

Application::Application()
{
    wsi = std::make_unique<WSILoaderGLFW>();
    wsi->init();

    window = std::make_unique<WindowGLFW>(1366, 768);

    context =
        std::make_unique<Context>("Renderer", VERSION(0, 0, 0), VERSION(0, 0, 0), window->getRequiredExtensions());

    // for (const auto& physicalDevice : context->enumerateAvailablePhysicalDevices())
    // {
    // 	physicalDevices.emplace_back(std::make_shared<PhysicalDevice>(physicalDevice));
    // }
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