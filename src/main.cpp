#include <iostream>

#include "application.hpp"

int main()
{
	Application app;

	try
	{
		app.loop();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}