WindowGLFW::PresentationWindow(const GraphicsAPIE api, const uint32_t width, const uint32_t height)
		: width(width), height(height)
	{
		glfwInit();

		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		switch (api)
		{
		case GraphicsAPIE::OPENGL:
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			break;
		}
		case GraphicsAPIE::VULKAN:
		{
			// no api specified to create vulkan context
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			break;
		}
		default:
			throw std::runtime_error("Invalid graphics API");
		}

		handle = glfwCreateWindow(width, height, "Renderer", nullptr, nullptr);
		glfwGetFramebufferSize(handle, &framebufferWidth, &framebufferHeight);
	}
	WindowGLFW::~PresentationWindow()
	{
		swapchain.reset();
		surface.reset();
		glfwDestroyWindow(handle);
		glfwTerminate();
	}

	const std::vector<const char*> WindowGLFW::getRequiredExtensions() const
	{
		uint32_t count = 0;
		const char** extensions;

		extensions = glfwGetRequiredInstanceExtensions(&count);

		return std::vector<const char*>(extensions, extensions + count);
	}

	const Surface& WindowGLFW::createSurface(const VkInstance& instance)
	{
		VkSurfaceKHR surfaceHandle;
		glfwCreateWindowSurface(instance, handle, nullptr, &surfaceHandle);

		surface = std::make_unique<Surface>(instance, surfaceHandle);
		return *surface;
	}

	const Swapchain& WindowGLFW::createSwapchain(const Context& context)
	{
		swapchain = std::make_unique<Swapchain>(context.instance,
			surface->handle,
			context.deviceSelector->getPhysicalDevice(),
			context.deviceSelector->getLogicalDevice());
		return *swapchain;
	}

	inline void WindowGLFW::makeContextCurrent()
	{
		glfwMakeContextCurrent(handle);
	}

	inline bool WindowGLFW::shouldClose() const
	{
		return glfwWindowShouldClose(handle);
	}