#include <cassert>

#include <graphics/context.hpp>
#include <graphics/device/device.hpp>
#include <graphics/device/physical_device.hpp>
#include <graphics/instance.hpp>
#include <wsi/window_glfw.hpp>

#include "application.hpp"

Application::Application()
{
    wsi = std::make_unique<WSILoaderGLFW>();
    wsi->init();

    window = std::make_unique<WindowGLFW>(1366, 768);

    auto ext = window->getRequiredExtensions();
    ext.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    context = std::make_unique<Context>(ContextCreateInfoT{
        .applicationName = "Renderer",
        .applicationVersion = VERSION(0, 0, 0),
        .engineVersion = VERSION(0, 0, 0),
        .instanceExtensions = ext,
#ifdef ENABLE_VIDEO_TRANSCODE
        .deviceExtensions =
            {
                VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
                VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,
                VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
                VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
            },
#endif
    });
    m_instance = std::make_unique<Instance>(InstanceCreateInfoT{context.get()});

    for (const std::string &physicalDeviceName : m_instance->enumerateAvailablePhysicalDevices(false))
    {
        std::shared_ptr<PhysicalDevice> physicalDevice = std::make_shared<PhysicalDevice>(PhysicalDeviceCreateInfoT{
            .cx = context.get(), .inst = m_instance.get(), .deviceName = physicalDeviceName.c_str()});
        assert(physicalDevice);

        physicalDevices.push_back(physicalDevice);
        devices.emplace_back(physicalDevice->createDevice());
    }
}

Application::~Application()
{
    devices.clear();
    physicalDevices.clear();
    m_instance.reset();
    context.reset();

    window.reset();
    wsi.reset();
}

int Application::perFrame()
{
    if (window->shouldClose())
    {
        wsi->terminate();
        return 0;
    }

    window->swapBuffers();
    window->pollEvents();
    // TODO : threadpool poll main queue

    return 1;
}