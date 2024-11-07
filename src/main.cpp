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
        return (EXIT_FAILURE);
    }

    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    setupSignalHandlers();
    try
    {
        ServerManager serverManager(argv[1]);
        serverManager.run();
    }
    catch (const std::exception &e)
    {
        logger.logError(LOG_ERROR, e.what(), true);
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
