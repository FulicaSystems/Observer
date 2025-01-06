#include <stdexcept>

#include "graphics/context.hpp"
#include "graphics/surface.hpp"

#include "window_glfw.hpp"

int WSILoaderGLFW::init()
{
    return glfwInit();
}

void WSILoaderGLFW::terminate()
{
    glfwTerminate();
}

WindowGLFW::WindowGLFW(const uint32_t width, const uint32_t height, const bool resizable)
    : m_width(width), m_height(height), m_resizable(resizable)
{
    glfwWindowHint(GLFW_RESIZABLE, (int)m_resizable);

    // TODO : find a way to move these specific calls to a dll

    // switch (api)
    // {
    // case GraphicsApiE::OPENGL:
    // {
    // 	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    // 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 	break;
    // }
    // case GraphicsApiE::VULKAN:
    // {
    if (!glfwVulkanSupported())
        throw std::runtime_error("GLFW failed to find the Vulkan loader");

    // no api specified to create vulkan context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // 	break;
    // }
    // default:
    // 	throw std::runtime_error("Invalid graphics API");
    // }

    m_handle = glfwCreateWindow(m_width, m_height, "Renderer", nullptr, nullptr);

    glfwGetFramebufferSize(m_handle, &m_framebufferWidth, &m_framebufferHeight);
}
WindowGLFW::~WindowGLFW()
{
    // swapchain.reset();
    // surface.reset();
    glfwDestroyWindow(m_handle);
}

const std::vector<const char *> WindowGLFW::getRequiredExtensions() const
{
    uint32_t count = 0;
    const char **extensions;

    extensions = glfwGetRequiredInstanceExtensions(&count);

    return std::vector<const char *>(extensions, extensions + count);
}

void WindowGLFW::makeContextCurrent()
{
    glfwMakeContextCurrent(m_handle);
}

std::unique_ptr<Surface> WindowGLFW::createSurface(const Context &cx)
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(cx.getInstanceHandle(), m_handle, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Could not create window surface");

    return std::make_unique<Surface>(cx, surface);
}

bool WindowGLFW::shouldClose() const
{
    return glfwWindowShouldClose(m_handle);
}

void WindowGLFW::swapBuffers()
{
    glfwSwapBuffers(m_handle);
}

void WindowGLFW::pollEvents()
{
    glfwPollEvents();
}

// const Surface& WindowGLFW::createSurface(const Instance& instance)
//{
//	VkSurfaceKHR surface;
//	glfwCreateWindowSurface(instance->m_handle, m_handle, nullptr, &surface);

//	m_surface = std::make_unique<Surface>(instance->m_handle, surface);
//	return *m_surface;
//}