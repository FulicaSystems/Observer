#include <cassert>

#include <graphics/context.hpp>
#include <graphics/device/device.hpp>
#include <graphics/device/physical_device.hpp>
#include <graphics/instance.hpp>
#include <graphics/surface.hpp>
#include <graphics/swapchain.hpp>

#include <wsi/window_glfw.hpp>

#include "application.hpp"

Application::Application()
{
    m_wsi = std::make_unique<WSILoaderGLFW>();
    m_wsi->init();

    m_window = std::make_unique<WindowGLFW>(width, height);

    auto ext = m_window->getRequiredExtensions();
    ext.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    m_context = std::make_unique<ContextSDK>(ContextCreateInfoT{
        .applicationName = "Renderer",
        .applicationVersion = VERSION(0, 0, 0),
        .engineVersion = VERSION(0, 0, 0),
        .layers =
            {
                     "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor",
                     },
        .instanceExtensions = ext,
#ifdef ENABLE_VIDEO_TRANSCODE
        .deviceExtensions =
            {
                     VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME, VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,
                     VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
                     },
#endif
    });
    m_instance = std::make_unique<Instance>(InstanceCreateInfoT{m_context.get()});

    auto surface = m_window->createSurface(m_context.get(), m_instance.get());

    int i = -1;
    for (const std::string &physicalDeviceName : m_instance->enumerateAvailablePhysicalDevices(false))
    {
        ++i;

        std::shared_ptr<PhysicalDevice> physicalDevice = std::make_shared<PhysicalDevice>(PhysicalDeviceCreateInfoT{
            .cx = m_context.get(),
            .inst = m_instance.get(),
            .deviceName = physicalDeviceName.c_str(),
            .surface = surface,
        });
        assert(physicalDevice);

        m_physicalDevices.push_back(physicalDevice);
        m_devices.emplace_back(physicalDevice->createDevice());

        if (m_currentDeviceIndex < 0 && physicalDevice->getDeviceType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            m_currentDeviceIndex = i;
    }
    if (i < 0)
        i = 0;

    m_window->setSwapChain(m_devices[m_currentDeviceIndex]->createSwapChain(SwapChainCreateInfoT{
        .surface = surface,
        .surfaceFormat =
            {
                .format = VK_FORMAT_B8G8R8A8_UNORM,
                .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            },
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .extent =
            {
                .width = width,
                .height = height,
            },
    }));
}

Application::~Application()
{
    m_window.reset();
    m_wsi.reset();

    m_devices.clear();
    m_physicalDevices.clear();

    m_instance.reset();
    m_context.reset();
}

int Application::perFrame()
{
    if (m_window->shouldClose())
    {
        m_wsi->terminate();
        return 0;
    }

    m_window->swapBuffers();
    m_window->pollEvents();
    // TODO : threadpool poll main queue

    return 1;
}