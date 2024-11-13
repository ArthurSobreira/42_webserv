#include "GetResponse.hpp"

/* Constructor Method */
GetResponse::GetResponse( std::string filePath )
	: Response(), _filePath(filePath) {};

/* Destructor Method */
GetResponse::~GetResponse( void ) {};

/* Public Methods */
void GetResponse::listDirectoryHandler( void ) {
		std::cout << "debbug response 6" << std::endl;
		_body = "<html><head><title>Index of " + _filePath + 
			"</title></head><body><h1>Index of " + _filePath + "</h1><hr><pre>";
		// _body = listDirectory(_filePath);
		_headers["Content-Type"] = "text/html";
		std::stringstream ss;
		ss << _body.size();
		_headers["Content-Length"] = ss.str();
		_reasonPhrase = "OK";
		_statusCode = "200";
}

void GetResponse::prepareResponse( const LocationConfigs &location ) {
	status Status;
	_filePath = location.root + _filePath;
	if (_filePath[_filePath.size() - 1] == '/' && !location.autoindex) {
		_filePath += location.index;
	}
	if (stat(_filePath.c_str(), &Status) != 0) {
		handleError("404", location.server->errorPages.at("404"), ERROR_NOT_FOUND);
		return;
	}
	if (S_ISDIR(Status.st_mode)) {
		listDirectoryHandler();
		return;
	}
	if (access(_filePath.c_str(), R_OK) != 0) {
		handleError("403", location.server->errorPages.at("403"), ERROR_FORBIDDEN);
		return;
	}
	handleFileResponse(_filePath);
}
