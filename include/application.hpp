#pragma once

#include <vector>

#include <GLFW/glfw3.h>

#include "renderer.hpp"

class Application
{
private:
	GLFWwindow* window;

	Renderer rdr;

	void windowInit();

public:
	Application();
	~Application();

	void loop();
};