#include "includes.hpp"

int main(int argc, char **argv)
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return 1;
	}
	setLogger(logger);
	unitTestLogClass();
	

return 0;
}