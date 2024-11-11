#include "DeleteResponse.hpp"

/* Constructor Method */
DeleteResponse::DeleteResponse( const std::string &filePath, ServerConfigs &server ) 
	: Response(), _filePath(filePath), _server(server) {}

/* Destructor Method */
DeleteResponse::~DeleteResponse( void ) {}

/* Public Methods */
void DeleteResponse::prepareResponse( void ) {
	if (access(_filePath.c_str(), F_OK) != 0) {
		handleError("404", _server.errorPages.at("404"), ERROR_NOT_FOUND, _logger);
		return;
	}
	if (access(_filePath.c_str(), W_OK) != 0) {
		handleError("403", _server.errorPages.at("403"), ERROR_FORBIDDEN, _logger);
		return;
	}
	if (remove(_filePath.c_str()) == 0) {
		_statusCode = "204";
		_reasonPhrase = "No Content";
	}
	else {
		handleError("500", _server.errorPages.at("500"), ERROR_INTERNAL_SERVER, _logger);
	}
}
