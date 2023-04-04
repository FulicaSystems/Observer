#pragma once

#include <vector>

#include <GLFW/glfw3.h>

#include "renderer.hpp"
#include "resourcesmanager.hpp"

class Application
{
private:
	GLFWwindow* window;

	Renderer rdr;
	ResourcesManager rmgr;

	void windowInit();

public:
	Application();
	~Application();

	void loop();
};