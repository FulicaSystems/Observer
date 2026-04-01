// TODO : clean la compilation et l’installation (glslc, install)
// TODO : tester allocator custom
// TODO : memory leaks (trouver alternative à Valgrind (Linux))
// TODO : texture
// TODO : mvp
// TODO : assimp
// TODO : option pour retirer des api de la compilation
// TODO : option pour changer d'api en runtime
// TODO : opengl
// TODO : direct 3d 11
// TODO : direct 3d 12

#include <iostream>

#include "client/application.hpp"

// argv[0] : exe path
// argv[1] : first parameter
int main(int argc, char** argv)
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
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        Application app /*(api >= 0 ? (GraphicsApiE)api : GraphicsApiE::VULKAN)*/;
        while (app.perFrame())
        {
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
