#pragma once

#include <memory>
#include <vector>

#include <GLFW/glfw3.h>

class WSILoaderI
{
public:
	virtual int init() = 0;
	virtual void terminate() = 0;
};

class WSILoaderGLFW : public WSILoaderI
{
public:
	int init() override;
	void terminate() override;
};

class Window
{

};

class WindowGLFW : public Window
{
private:
	//std::unique_ptr<Swapchain> m_swapchain;
	//std::unique_ptr<Surface> m_surface;

	bool m_resizable = false;

	uint32_t m_width = 1366;
	uint32_t m_height = 768;

	int m_framebufferWidth;
	int m_framebufferHeight;

	GLFWwindow* m_handle;

public:
	WindowGLFW(const uint32_t width, const uint32_t height, const bool resizable = false);
	~WindowGLFW();

	const std::vector<const char*> getRequiredExtensions() const;

	void makeContextCurrent();
	void swapBuffers();
	void pollEvents();
	bool shouldClose() const;

	//const Surface& createSurface(const Instance& instance);
};