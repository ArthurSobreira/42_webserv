
#include "CGI.hpp"

/* Constructor Method */
CGI::CGI( const Request &request, const LocationConfigs &location ) 
	: _returnCode(200), _returnbody(DEFAULT_EMPTY), 
	_request(request), _cgiConfig(location.cgiConfig) {
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);

	if (_cgiConfig.cgiEnabled) {
		_cgiPath = location.root + "/" + _cgiConfig.cgiPath;
		if (location.cgiConfig.cgiExtension == EXTENSION_PHP) {
			_cgiExecutable = PHP_EXECUTABLE;
		} else if (location.cgiConfig.cgiExtension == EXTENSION_PY) {
			_cgiExecutable = PYTHON_EXECUTABLE;
		}

		_setEnvironmentVars();

		logger.logDebug(LOG_INFO, "CGI Path: " + _cgiPath, true);
		logger.logDebug(LOG_INFO, "CGI Executable: " + _cgiExecutable, true);
		logger.logDebug(LOG_INFO, "CGI SERVER_PROTOCOL: " + _env["SERVER_PROTOCOL"], true);
		logger.logDebug(LOG_INFO, "CGI REQUEST_METHOD: " + _env["REQUEST_METHOD"], true);
		logger.logDebug(LOG_INFO, "CGI REQUEST_URI: " + _env["REQUEST_URI"], true);
		logger.logDebug(LOG_INFO, "CGI SCRIPT_NAME: " + _env["SCRIPT_NAME"], true);
		logger.logDebug(LOG_INFO, "CGI SCRIPT_FILENAME: " + _env["SCRIPT_FILENAME"], true);
		logger.logDebug(LOG_INFO, "CGI PATH_INFO: " + _env["PATH_INFO"], true);
		logger.logDebug(LOG_INFO, "CGI QUERY_STRING: " + _env["QUERY_STRING"], true);
	}
	(void)_returnCode;
}

/* Destructor Method */
CGI::~CGI( void ) {};

/* Private Methods */
void	CGI::_setEnvironmentVars( void ) {
	_env["SERVER_PROTOCOL"] = _request.getHttpVersion();
	_env["REQUEST_METHOD"] = _request.getMethod();
	_env["REQUEST_URI"] = _request.getUri();
	_env["SCRIPT_NAME"] = _cgiConfig.cgiPath;
	_env["SCRIPT_FILENAME"] = _cgiPath;
	_env["PATH_INFO"] = _getPathInfo(_request.getUri());
	_env["QUERY_STRING"] = _getQueryString(_request.getUri());
}

std::string	CGI::_getQueryString( const std::string &uri ) const {
	std::size_t pos = uri.find('?');
	if (pos != std::string::npos) { 
		return uri.substr(pos + 1); 
	} else { return DEFAULT_EMPTY; }
}

std::string	CGI::_getPathInfo( const std::string &uri ) const {
	std::string scriptName = _cgiConfig.cgiPath;
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
