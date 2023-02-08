#include <iostream>

#include "application.hpp"

#define CHKLK_OVERRIDE_NEW_OP
#include "utils/checkleak.hpp"
#include <Windows.h>

int main()
{
	// TODO : check memory leaks
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

	CHKLK_APP
	return EXIT_SUCCESS;
}