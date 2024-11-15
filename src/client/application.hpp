#pragma once

#include <memory>

#include "wsi/window.hpp"
#include "graphics/context.hpp"

class Application
{
private:
	std::unique_ptr<WSILoaderI> wsi;
	std::unique_ptr<WindowGLFW> window;
	std::unique_ptr<Context> context;

public:
	Application();

	void run();
};