# TODO : render in fbo instead of swapchain image directly (google search "bgra vs rgba")
# TODO : clean la compilation et l’installation (glslc, install)


#include <iostream>

#include "client/application.hpp"

#define CHKLK_OVERRIDE_NEW_OP
#include "checkleak.hpp"
#include <Windows.h>

// argv[0] : exe path
// argv[1] : first parameter
int main(int argc, char **argv)
{
    int api = 0;
    // TODO : make a standalone argument parser for libraries
    try
    {
        // if parameters are specified
        if (argc > 1)
        {
            for (int i = 1; i < argc; ++i)
            {
                if (argv[i][0] != '-')
                {
                    char msg[128] = "Invalid executable argument (must start with a dash '-') : ";
                    throw std::runtime_error(strcat(msg, argv[i]));
                }

                std::string str = std::string(argv[i]);
                if (str == "-gl" || str == "-opengl")
                    api = 1;

                if (str == "-vk" || str == "-vulkan")
                    api = 2;
            }
        }
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    CHKLK_INIT

    CHKLK_ENTRY_SNAP
    {
        try
        {
            Application app /*(api >= 0 ? (GraphicsApiE)api : GraphicsApiE::VULKAN)*/;
            app.run();
        }
        catch (const std::exception &ex)
        {
            std::cerr << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
    CHKLK_EXIT_DIFF

    // CHKLK_APP
    return EXIT_SUCCESS;
}
