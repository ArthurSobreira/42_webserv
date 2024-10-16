
#include "CGI.hpp"

namespace CGIUtils {
	bool	methodIsOnLocation( LocationConfigs &location, 
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

	std::string	intToString(int value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
}

/* Constructor Method */
CGI::CGI( const Request &request, const ServerConfigs &server,
	const LocationConfigs &location ) 
	: _returnCode(200), _returnbody(DEFAULT_EMPTY), _request(request), 
	_serverConfig(server), _locationConfig(location),
	_logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE) {

	if (_locationConfig.cgiEnabled) {
		_cgiPath = location.root + "/" + location.cgiPath;
		if (location.cgiExtension == EXTENSION_PHP) {
			_cgiExecutable = PHP_EXECUTABLE;
		} else if (location.cgiExtension == EXTENSION_PY) {
			_cgiExecutable = PYTHON_EXECUTABLE;
		}
		this->_setEnvironmentVars();

		_logger.logDebug(LOG_INFO, "CGI Body: " + _request.getBody(), true);
		_logger.logDebug(LOG_INFO, "CGI Path: " + _cgiPath, true);
		_logger.logDebug(LOG_INFO, "CGI Executable: " + _cgiExecutable, true);
		_logger.logDebug(LOG_INFO, "CGI SERVER_PROTOCOL: " + _env["SERVER_PROTOCOL"], true);
		_logger.logDebug(LOG_INFO, "CGI REQUEST_METHOD: " + _env["REQUEST_METHOD"], true);
		_logger.logDebug(LOG_INFO, "CGI REQUEST_URI: " + _env["REQUEST_URI"], true);
		_logger.logDebug(LOG_INFO, "CGI SCRIPT_NAME: " + _env["SCRIPT_NAME"], true);
		_logger.logDebug(LOG_INFO, "CGI SCRIPT_FILENAME: " + _env["SCRIPT_FILENAME"], true);
		_logger.logDebug(LOG_INFO, "CGI PATH_INFO: " + _env["PATH_INFO"], true);
		_logger.logDebug(LOG_INFO, "CGI QUERY_STRING: " + _env["QUERY_STRING"], true);
		_logger.logDebug(LOG_INFO, "CGI RAW_REQUEST: " + _env["RAW_REQUEST"], true);
		_logger.logDebug(LOG_INFO, "CGI CONTENT_LENGTH: " + _env["CONTENT_LENGTH"], true);
		_logger.logDebug(LOG_INFO, "CGI CONTENT_TYPE: " + _env["CONTENT_TYPE"], true);
	}
}

/* Destructor Method */
CGI::~CGI( void ) {};

/* Private Methods */
void	CGI::_setEnvironmentVars( void ) {
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

char	**CGI::_generateEnvp( void ) {
	char **envp = new char*[_env.size() + 1];
	int index = 0;

	for (std::map<std::string, std::string>::iterator it = _env.begin(); 
		it != _env.end(); ++it) {
		std::string envVar = it->first + "=" + it->second;
		envp[index] = new char[envVar.length() + 1];
		std::strcpy(envp[index], envVar.c_str());
		index++;
	}
	envp[index] = NULL;
	return (envp);
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

void	CGI::_handleCGIError( int code, const std::string &message ) {
	Response response;
	std::string codeStr;

	_returnCode = code;
	_locationConfig.cgiEnabled = false;
	codeStr = CGIUtils::intToString(_returnCode);
	_logger.logError(LOG_ERROR, message, true);
	response.handleError(_returnCode, 
		_serverConfig.errorPages[codeStr], message, _logger);
	_returnbody = response.generateResponse();
}

/* Public Methods */
int	CGI::getReturnCode( void ) const {
	return (_returnCode);
}

std::string	CGI::getReturnBody( void ) const {
	return (_returnbody);
}

void	CGI::executeCGI( void ) {
	if (access(_cgiPath.c_str(), X_OK) == -1) {
		_handleCGIError(403, ERROR_FORBIDDEN);
	} else if (!CGIUtils::methodIsOnLocation(_locationConfig, 
		_request.getMethod())) {
		_handleCGIError(405, ERROR_METHOD_NOT_ALLOWED);
	}
	char **envp = _generateEnvp();
	int	pipefd[2];
	int status;

	if (pipe(pipefd) == -1) {
		_handleCGIError(500, "Error creating pipe");
		return ;
	}
	pid_t pid = fork();
	if (pid == -1) {
		_handleCGIError(500, "Error forking process");
		return ;
	} else if (pid == 0) {
		char *argv[] = { const_cast<char *>(_cgiExecutable.c_str()), 
			const_cast<char *>(_cgiPath.c_str()), NULL };

		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execve(_cgiExecutable.c_str(), argv, envp) == -1) { // caso php não exista isso não funciona
			_handleCGIError(500, "Error executing CGI script");
			exit(EXIT_FAILURE);
		}
	} else {
		close(pipefd[1]);
		char buffer[4096];
		size_t bytes_read;

		while ((bytes_read = read(pipefd[0], buffer, 4096)) > 0) {
			_returnbody.append(buffer, bytes_read);
		}
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			// _returnCode = WEXITSTATUS(status);  // naõ tenho certeza se está correto
			_returnCode = 200;
		}
		close(pipefd[0]);
	}
	for (int i = 0; envp[i] != NULL; ++i) {
		delete[] envp[i];
	}
	delete[] envp;
}
