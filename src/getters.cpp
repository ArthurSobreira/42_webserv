#include "includes.hpp"


Logger &getLogger()
{
	static Logger logger("logs/server.log");
	return logger;
}
