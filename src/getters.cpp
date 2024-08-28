#include "includes.hpp"


Logger &getLogger()
{
	static Logger logger("logs/server.log");
	return logger;
}

HttpError &getError404(){
	static HttpError error(404);
	return error;
}

