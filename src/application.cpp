#include "utils/multithread/globalthreadpool.hpp"

#include "application.hpp"

void Application::run()
{
	while (!window->shouldClose())
	{
		glfwPollEvents();
		Utils::GlobalThreadPool::pollMainQueue();
	}
}