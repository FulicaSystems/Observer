#include <iostream>

#include "application.hpp"

int main()
{
	Application app;

	try
	{
		app.loop();

		//vkDeviceWaitIdle(device);
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	app.destroy();

	return EXIT_SUCCESS;
}