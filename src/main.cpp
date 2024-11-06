#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "Globals.hpp"
#include "EpollManager.hpp"
#include "Fds.hpp"
#include "ServerManager.hpp"

volatile sig_atomic_t stop = 0;


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
        return EXIT_FAILURE;
    }

    setupSignalHandlers();

    try
    {
        ServerManager serverManager(argv[1]);
        serverManager.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}