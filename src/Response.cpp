#include "Response.hpp"

Response::Response( void ) : _body(DEFAULT_EMPTY), _statusCode("200"), 
	_reasonPhrase("OK"), _logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE) {}

Response::~Response( void ) {}

std::string Response::generateResponse() const
{
	std::ostringstream response_stream;

	response_stream << "HTTP/1.1 " << _statusCode << " " << _reasonPhrase << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		response_stream << it->first << ": " << it->second << "\r\n";
	}
	response_stream << "\r\n";
	response_stream << _body;

	return response_stream.str();
}

void Response::handleError(std::string status_code, const std::string &error_page, const std::string &error_message, Logger &logger)
{
	_statusCode = status_code;
	_reasonPhrase = error_message;
	_body = readFile(error_page);
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	_headers["Content-Type"] = "text/html";
	logger.logError(LOG_ERROR, "Error: " + error_message);
}

void Response::handleFileResponse(const std::string &path, Logger &logger)
{
	_statusCode = "200";
	_reasonPhrase = "OK";
	if (_body.empty())
		_body = readFile(path);
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	_headers["Content-Type"] = getContentType(path);
	logger.logDebug(LOG_INFO, "Response body: " + _body);
}
