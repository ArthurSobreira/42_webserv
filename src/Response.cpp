#include "Response.hpp"

/* Constructor Method */
Response::Response( void ) : _body(DEFAULT_EMPTY), 
	_statusCode("200"), _reasonPhrase("OK") {};

/* Destructor Method */
Response::~Response( void ) {};

/* Public Methods */
std::string Response::generateResponse( void ) const {
	std::ostringstream response_stream;

	response_stream << "HTTP/1.1 " << _statusCode << " " << _reasonPhrase << "\r\n";
	for (stringMap::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		response_stream << it->first << ": " << it->second << "\r\n";
	}
	response_stream << "\r\n";
	response_stream << _body;

	return response_stream.str();
}

void	Response::handleError( std::string status_code, const std::string &error_page, 
	const std::string &error_message ) {
	_statusCode = status_code;
	_reasonPhrase = error_message;
	_body = readFile(error_page);
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	_headers["Content-Type"] = "text/html";
	logger.logError(LOG_ERROR, "Error: " + error_message, true);
}

void	Response::handleFileResponse( const std::string &path ) {
	_statusCode = "200";
	_reasonPhrase = "OK";
	if (_body.empty()) {
		_body = readFile(path);
	}
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	if (path.empty()) {
		_headers["Content-Type"] = "text/html";
	} else {
		_headers["Content-Type"] = getContentType(path);
	}
	logger.logDebug(LOG_INFO, "Response body: " + _body);
}
