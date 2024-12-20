#include "Includes.hpp"
#include "Defines.hpp"
#include "Utils.hpp"
#include "ServerManager.hpp"

Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
volatile sig_atomic_t stop = 0;

int main(int argc, char **argv) {
    if (argc != 2) {
        logger.logError(LOG_ERROR, "Usage: " + std::string(argv[0]) + 
            " <config_file_path>", true);
        return (EXIT_FAILURE);
    }
    setupSignalHandlers();
    try {
        ServerManager serverManager(argv[1]);
        serverManager.run();
    }
    catch (const std::exception &e) {
        logger.logError(LOG_ERROR, e.what(), true);
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
