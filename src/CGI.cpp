
#include "CGI.hpp"

static bool	methodIsOnLocation( LocationConfigs &location, 
	const std::string &method ) {
	std::vector<httpMethod> methods = location.methods;

	if (methods.empty()) { return true; }
	for (std::vector<httpMethod>::iterator it = methods.begin(); 
		it != methods.end(); ++it) {
		if (*it == GET && method == "GET") { return true; }
		if (*it == POST && method == "POST") { return true; }
		if (*it == DELETE && method == "DELETE") { return true; }
	}
	return (false);
}

static std::string	intToString(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

void	CGI::_handleCGIError( int code, const std::string &message, Logger &logger ) {
	Response response;
	std::string codeStr;

	_returnCode = code;
	_locationConfig.cgiEnabled = false;
	codeStr = intToString(_returnCode);
	logger.logError(LOG_WARN, message, true);
	response.handleError(_returnCode, 
		_serverConfig.errorPages[codeStr], message, logger);
	_returnbody = response.generateResponse();
}

/* Constructor Method */
CGI::CGI( const Request &request, const ServerConfigs &server,
	const LocationConfigs &location ) 
	: _returnCode(200), _returnbody(DEFAULT_EMPTY), _request(request), 
	_serverConfig(server), _locationConfig(location) {
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);

	if (access(_locationConfig.cgiPath.c_str(), R_OK) == -1) {  // Não está funcionando :(
		_handleCGIError(404, ERROR_NOT_FOUND, logger);
	} else if (!methodIsOnLocation(_locationConfig, _request.getMethod())) {
		_handleCGIError(405, ERROR_METHOD_NOT_ALLOWED, logger);
	}

	if (_locationConfig.cgiEnabled) {
		_cgiPath = location.root + "/" + location.cgiPath;
		if (location.cgiExtension == EXTENSION_PHP) {
			_cgiExecutable = PHP_EXECUTABLE;
		} else if (location.cgiExtension == EXTENSION_PY) {
			_cgiExecutable = PYTHON_EXECUTABLE;
		}

		_setEnvironmentVars();

		logger.logDebug(LOG_INFO, "CGI Body: " + _request.getBody(), true);
		logger.logDebug(LOG_INFO, "CGI Path: " + _cgiPath, true);
		logger.logDebug(LOG_INFO, "CGI Executable: " + _cgiExecutable, true);
		logger.logDebug(LOG_INFO, "CGI SERVER_PROTOCOL: " + _env["SERVER_PROTOCOL"], true);
		logger.logDebug(LOG_INFO, "CGI REQUEST_METHOD: " + _env["REQUEST_METHOD"], true);
		logger.logDebug(LOG_INFO, "CGI REQUEST_URI: " + _env["REQUEST_URI"], true);
		logger.logDebug(LOG_INFO, "CGI SCRIPT_NAME: " + _env["SCRIPT_NAME"], true);
		logger.logDebug(LOG_INFO, "CGI SCRIPT_FILENAME: " + _env["SCRIPT_FILENAME"], true);
		logger.logDebug(LOG_INFO, "CGI PATH_INFO: " + _env["PATH_INFO"], true);
		logger.logDebug(LOG_INFO, "CGI QUERY_STRING: " + _env["QUERY_STRING"], true);
		logger.logDebug(LOG_INFO, "CGI RAW_REQUEST: " + _env["RAW_REQUEST"], true);
		logger.logDebug(LOG_INFO, "CGI CONTENT_LENGTH: " + _env["CONTENT_LENGTH"], true);
		logger.logDebug(LOG_INFO, "CGI CONTENT_TYPE: " + _env["CONTENT_TYPE"], true);
	}
	(void)_returnCode;
}

/* Destructor Method */
CGI::~CGI( void ) {};

/* Private Methods */
void	CGI::_setEnvironmentVars( void ) {
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);

	_env["SERVER_PROTOCOL"] = _request.getHttpVersion();
	_env["REQUEST_METHOD"] = _request.getMethod();
	_env["REQUEST_URI"] = _request.getUri();
	_env["SCRIPT_NAME"] = _locationConfig.cgiPath;
	_env["SCRIPT_FILENAME"] = _cgiPath;
	_env["PATH_INFO"] = _getPathInfo(_request.getUri());
	_env["QUERY_STRING"] = _getQueryString(_request.getUri());
	if (_request.getMethod() == "POST") {
		std::string contentLength = _request.getHeader("Content-Length");
		std::string contentType = _request.getHeader("Content-Type");

		if (!contentLength.empty()) {
			_env["CONTENT_LENGTH"] = contentLength;
		} else { _env["CONTENT_LENGTH"] = DEFAULT_EMPTY; }

		if (!contentType.empty()) {
			_env["CONTENT_TYPE"] = contentType;
		} else { _env["CONTENT_TYPE"] = "text/plain"; }
	}
}

std::string CGI::_getContentLength( void ) const {
	std::string contentLength = _request.getHeader("Content-Length");

	if (!contentLength.empty()) { return contentLength; }
	else { return DEFAULT_EMPTY; }
}

std::string	CGI::_getQueryString( const std::string &uri ) const {
	std::size_t pos = uri.find('?');
	if (pos != std::string::npos) { 
		return uri.substr(pos + 1); 
	} else { return DEFAULT_EMPTY; }
}

std::string	CGI::_getPathInfo( const std::string &uri ) const {
	std::string scriptName = _locationConfig.cgiPath;
	std::size_t scriptPos = uri.find(scriptName);

	if (scriptPos != std::string::npos) {
		std::string pathInfo = uri.substr(scriptPos + scriptName.length());

		std::size_t queryPos = pathInfo.find('?');
		if (queryPos != std::string::npos) {
			pathInfo = pathInfo.substr(0, queryPos);
		}
		return (pathInfo.empty() ? "/" : pathInfo);
	}
	return (DEFAULT_LOCATION_PATH);
}

/* Public Methods */
int	CGI::getReturnCode( void ) const {
	return (_returnCode);
}

std::string	CGI::getReturnBody( void ) const {
	return (_returnbody);
}
