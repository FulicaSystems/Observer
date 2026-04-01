#include <cassert>

#include <graphics/context.hpp>
#include <graphics/device/device.hpp>
#include <graphics/device/memory/image.hpp>
#include <graphics/device/physical_device.hpp>
#include <graphics/instance.hpp>
#include <graphics/surface.hpp>
#include <graphics/swapchain.hpp>

#include <wsi/window_glfw.hpp>

#include <renderer/renderer.hpp>

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
        // TODO : get the version numbers from cmake
        .applicationVersion = VERSION(0, 0, 0),
        .engineVersion = VERSION(0, 0, 0),
        .layers =
            {
                     "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor",
                     },
        .instanceExtensions = ext,
        .deviceExtensions =
            {
                     VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef ENABLE_VIDEO_TRANSCODE
                     VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME, VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,
                     VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME, VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
#endif
                     },
    });
    m_instance = std::make_unique<Instance>(InstanceCreateInfoT{m_context.get()});

    auto surface = m_window->createSurface(m_context.get(), m_instance.get());

    int i = -1;
    for (const std::string& physicalDeviceName :
         m_instance->enumerateAvailablePhysicalDevices(false))
    {
        ++i;

        std::shared_ptr<PhysicalDevice> physicalDevice =
            std::make_shared<PhysicalDevice>(PhysicalDeviceCreateInfoT{
                .cx = m_context.get(),
                .inst = m_instance.get(),
                .deviceName = physicalDeviceName.c_str(),
                .surface = surface,
            });
        assert(physicalDevice);

        m_physicalDevices.push_back(physicalDevice);
        m_devices.emplace_back(physicalDevice->createDevice());

        if (m_currentDeviceIndex < 0 &&
            physicalDevice->getDeviceType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            m_currentDeviceIndex = i;
    }

    auto backendCreateInfo = std::make_shared<LegacyRendererBackendCreateInfoT>();
    backendCreateInfo->bufferingType = BufferingTypeE::DOUBLE_BUFFERING;
    backendCreateInfo->device = m_devices[m_currentDeviceIndex].get();
    backendCreateInfo->renderPass = {
        m_devices[m_currentDeviceIndex]->createRenderPass(RenderPassCreateInfoT{
                                                                                .colorAttachments =
                {
                    Attachment{VkAttachmentDescription{
                                   .format = VK_FORMAT_B8G8R8A8_UNORM,
                                   .samples = VK_SAMPLE_COUNT_1_BIT,
                                   .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                   .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                   .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                   .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                   .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                   .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                               },
                               VkAttachmentReference{
                                   .attachment = 0,
                                   .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                               }},
                }, .depthAttachment = std::make_optional<Attachment>(
                {VkAttachmentDescription{
                     .format = VK_FORMAT_D32_SFLOAT_S8_UINT,
                     .samples = VK_SAMPLE_COUNT_1_BIT,
                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                     .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                     .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                 },
                 VkAttachmentReference{
                     .attachment = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}}),
                                                                                .subpasses =
                {
                    SubpassDescriptionT{
                        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                        .colorAttachmentIndices = {0},
                    },
                }, .dependencies =
                {
                    VkSubpassDependency{
                        .srcSubpass = VK_SUBPASS_EXTERNAL,
                        .dstSubpass = 0,
                        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                        .srcAccessMask = 0,
                        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                         VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    },
                }, }
        )
    };
    m_renderer = std::make_unique<Renderer>(
        RendererCreateInfoT{.device = m_devices[m_currentDeviceIndex].get(),
                            .backend = std::make_unique<LegacyRendererBackend>(backendCreateInfo)});

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
        .viewCreateInfo =
            ImageViewCreateInfoT{
                            .aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                            },
        .renderPass = backendCreateInfo->renderPass,
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

    m_window->pollEvents();
    // TODO : threadpool poll main queue

    m_renderer->render(/*const Scene& scene*/);

    m_window->swapBuffers();
    m_renderer->swap();
    return 1;
}