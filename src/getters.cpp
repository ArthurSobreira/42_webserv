#include "includes.hpp"

Logger *logger = NULL;

void	setLogger(Logger &log)
{
	if (logger == NULL)
		logger = &log;
}

Logger &getLogger()
{
	return *logger;
}