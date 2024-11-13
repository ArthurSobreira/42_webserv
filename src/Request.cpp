#include "Request.hpp"
#include "Utils.hpp"
#include "Includes.hpp"

/* Constructor Method */
Request::Request( const std::string &rawRequest, 
	bool completeRequest) : _rawRequest(rawRequest)
{
	_method = INVALID;
	_isCGI = false;
	_isRedirect = false;
	_connectionClose = false;
	if (completeRequest) {
		_parseRequest();
		_checkConnectionClose();
	}
}

/* Destructor Method */
Request::~Request( void ) {};

/* Getter Method */
const std::string	&Request::getHeader( const std::string &name ) const {
	stringMap::const_iterator it = _headers.find(name);
	if (it == _headers.end())
	{
		static const std::string empty;
		return empty;
	}
	return it->second;
}

/* Public Method */
std::string	Request::validateRequest( Config _config, ServerConfigs server, 
	bool completeRequest ) {
	std::string error = DEFAULT_EMPTY;
	bool locationFound = false;
	std::string currentUri = _uri;

	if (!completeRequest) {
		return "413";
	}
	while (!locationFound && !currentUri.empty()) {
		_location = _config.getLocationConfig(server, currentUri, locationFound);
		if (!locationFound) {
			currentUri = _folderPath(currentUri);
		}
	}
	if (!locationFound) {
		error = "404";
	} else if (std::find(_location.methods.begin(), _location.methods.end(), 
		getMethod()) == _location.methods.end()) {
		error = "405";
	} else if (!_validateHost(server)) {
		error = "400";
	}
	_isRedirect = _location.redirectSet;
	_isCGI = _location.cgiEnabled;
	return error;
}

/* Private Methods */
void	Request::_parseRequest( void ) {
	std::istringstream requestStream(_rawRequest);
	std::string line;
	std::vector<std::string> headerLines;

	size_t pos = _rawRequest.find("\r\n\r\n");
	if (pos != std::string::npos) {
		std::string headersPart = _rawRequest.substr(0, pos);
		_body = _rawRequest.substr(pos + 4);
		std::istringstream headersStream(headersPart);
		bool isFirstLine = true;
		while (std::getline(headersStream, line)) {
			if (line.empty() || line == "\r") {
				continue;
			}
			if (isFirstLine) {
				_parseMethodAndUri(line);
				isFirstLine = false;
			} else {
				headerLines.push_back(line);
			}
		}
		_parseHeaders(headerLines);
	}
	_parseBody();
}

void	Request::_parseMethodAndUri( const std::string &line ) {
	std::istringstream lineStream(line);
	std::string method;

	lineStream >> method >> _uri >> _version;

	_parserQueryString();
	_method = _parseMethod(method);
	if (_uri != "/") {
		_uri = removeLastSlashes(_uri);
	}
	logger.logDebug(LOG_INFO, "Request: " + getStringMethod(getMethod())
	+ " " + getUri() + " " + getVersion(), true);
}

void	Request::_parserQueryString( void ) {
	size_t pos = _uri.find("?");
	if (pos != std::string::npos) {
		_queryString = _uri.substr(pos + 1);
		_uri = _uri.substr(0, pos);
	}
}

void	Request::_parseHeaders( const std::vector<std::string> &headerLines ) {
	for (std::vector<std::string>::const_iterator it = headerLines.begin(); 
		it != headerLines.end(); ++it) {
		size_t colonPos = it->find(':');
		if (colonPos != std::string::npos)
		{
			std::string name = it->substr(0, colonPos);
			std::string value = it->substr(colonPos + 2);
			_headers[name] = value;
		}
	}
}

void	Request::_parseBody( void ) {
	if (_headers.find("Content-Length") == _headers.end()) {
		return;
	}
	logger.logDebug(LOG_DEBUG, "Content-Length: " + _headers["Content-Length"]);
	logger.logDebug(LOG_DEBUG, "Body size: " + intToString(_body.size()));
	if (_headers.find("Content-Type") != _headers.end()) {
		std::string contentType = _headers["Content-Type"];
		if (contentType.find("multipart/form-data") != std::string::npos) {
			size_t pos = contentType.find("boundary=");
			if (pos != std::string::npos)
			{
				_headers["boundary"] = contentType.substr(pos + 9);
				_extractMultipartNamesAndFilenames();
			}
		}
	}
}

void	Request::_extractMultipartNamesAndFilenames( void ) {
	std::string boundary = "--" + _headers["boundary"];
	std::string endBoundary = boundary + "--";
	size_t pos = _body.find(boundary);
	size_t endPos = _body.find(endBoundary);
	while (pos != std::string::npos && pos < endPos)
	{
		size_t namePos = _body.find("name=\"", pos);
		size_t filenamePos = _body.find("filename=\"", pos);
		if (namePos != std::string::npos && filenamePos != std::string::npos)
		{
			namePos += 6;
			filenamePos += 10;
			size_t nameEnd = _body.find("\"", namePos);
			size_t filenameEnd = _body.find("\"", filenamePos);
			std::string name = _body.substr(namePos, nameEnd - namePos);
			std::string filename = _body.substr(filenamePos, filenameEnd - filenamePos);
			_headers["name"] = name;
			_headers["filename"] = filename;
		}
		pos = _body.find(boundary, pos + 1);
	}
	logger.logDebug(LOG_DEBUG, "Name: " + _headers["name"]);
	logger.logDebug(LOG_DEBUG, "Filename: " + _headers["filename"]);
}

std::string	Request::_folderPath( const std::string &uri ) {
	if (uri == "/") {
		return uri;
	}
	std::string folderPath = uri;
	if (isDirectory(folderPath) && folderPath[folderPath.size() - 1] != '/') {
		folderPath += "/";
	}
	if (counterOneSlash(folderPath)) {
		folderPath = "/";
		return folderPath;
	}
	if (folderPath[folderPath.size() - 1] == '/') {
		folderPath = folderPath.substr(0, folderPath.size() - 1);
	}
	size_t pos = folderPath.find_last_of('/');
	if (pos != std::string::npos) {
		folderPath = folderPath.substr(0, pos);
	}
	return folderPath;
}

bool	Request::_validateHost( ServerConfigs server ) {
	std::string host = getHeader("Host");
	if (host.empty()) {
		return false;
	}
	std::string serverName = host;
	if (serverName.find(':') != std::string::npos) {
		serverName = serverName.substr(0, serverName.find(':'));
	}
	if (server.serverName != serverName && 
		serverName != DEFAULT_SERVER_NAME && 
		serverName != DEFAULT_HOST) {
		return false;
	}
	return true;
}

void	Request::_checkConnectionClose( void ) {
	stringMap::const_iterator it = _headers.find("Connection");
	if (it != _headers.end() && it->second == "close") {
		_connectionClose = true;
	}
}

httpMethod	Request::_parseMethod( const std::string &method ) {
	if (method == "GET") { return GET; }
	else if (method == "POST") { return POST; }
	else if (method == "DELETE") { return DELETE; }
	return INVALID;
}
