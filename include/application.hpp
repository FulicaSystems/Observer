#pragma once

#include <vector>

#include "renderer.hpp"

// windowing lib should always be include after the api file
// #include <glad/gl.h> or <glad/vulkan.h>
// #include <GLFW/glfw3.h> or else

class Application
{
private:
	class GLFWwindow* window;

	Renderer rdr;

	void windowInit();

public:
	Application(const EGraphicsAPI graphicsApi = EGraphicsAPI::VULKAN);
	~Application();

	void loop();
};