#include "utils/multithread/globalthreadpool.hpp"

#include "application.hpp"

void Application::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		Utils::GlobalThreadPool::pollMainQueue();
	}
}