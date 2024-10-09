
#include "CGI.hpp"

/* Constructor Method */
CGI::CGI( const Request &request, const LocationConfigs &location ) 
	: _request(request), _cgiConfig(location.cgiConfig) {
	_returnCode = 0;
	_returnbody = "";

	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);

	if (_cgiConfig.cgiEnabled) {
		_cgiPath = location.root + "/" + _cgiConfig.cgiPath;
		if (location.cgiConfig.cgiExtension == EXTENSION_PHP) {
			_cgiExecutable = "/usr/bin/php-cgi";
		} else if (location.cgiConfig.cgiExtension == EXTENSION_PY) {
			_cgiExecutable = "/usr/bin/python3";
		}

		logger.logDebug(LOG_INFO, "CGI Path: " + _cgiPath, true);
		logger.logDebug(LOG_INFO, "CGI Executable: " + _cgiExecutable, true);

		// _cgiExecutable = location.cgiConfig.cgiPath;
		// _env["SERVER_PROTOCOL"] = request.getHttpVersion();
		// _env["REQUEST_METHOD"] = request.getMethod();
		// _env["REQUEST_URI"] = request.getUri();
		// _env["SCRIPT_NAME"] = location.cgiConfig.cgiPath;
		// _env["SCRIPT_FILENAME"] = location.root + location.cgiConfig.cgiPath;
		// _env["PATH_INFO"] = request.getUri();
	}
}

/* Destructor Method */
CGI::~CGI( void ) {};
