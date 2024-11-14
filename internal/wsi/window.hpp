#pragma once

#include <vulkan/vulkan.h>

class WindowGLFW
{
private:
	std::unique_ptr<Swapchain> swapchain;
	std::unique_ptr<Surface> surface;

public:
	// default extent : HD ready resolution
	uint32_t width = 1366;
	uint32_t height = 768;

	int framebufferWidth;
	int framebufferHeight;

	GLFWwindow* m_handle;

	PresentationWindow(const GraphicsAPIE api, const uint32_t width, const uint32_t height);
	~PresentationWindow();

	const std::vector<const char*> getRequiredExtensions() const;

	const Surface& createSurface(const VkInstance& instance);

	const Swapchain& createSwapchain(const Context& context);

	inline void makeContextCurrent();

	inline bool shouldClose() const;
};