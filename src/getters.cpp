#include "includes.hpp"

Logger &getLogger( void ) {
	static Logger logger(LOG_FILE);

	return (logger);
}

HttpError &getError404( void ) {
	static HttpError error(404);

	return (error);
}
