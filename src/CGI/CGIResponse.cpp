#include "CGIResponse.hpp"

/* Constructor Method */
CGIResponse::CGIResponse( const Request &request, const LocationConfigs &location ) 
	: Response(), _request(request), _locationConfig(location), 
	_logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE) {
	
	if (_locationConfig.cgiEnabled) {
		_cgiPath = location.root + "/" + location.cgiPath;
		_cgiExecutable = _getExecutable(location.cgiExtension);

		if (access(_cgiPath.c_str(), X_OK) == -1) {
			_handleCGIError(403, ERROR_FORBIDDEN);
		} else if (!CGIUtils::methodIsOnLocation(_locationConfig, 
			_request.getMethod())) {
			_handleCGIError(405, ERROR_METHOD_NOT_ALLOWED);
		} else if (CGIUtils::isUploadRequest(_request) && 
			!_locationConfig.uploadEnabled) {
			_handleCGIError(403, ERROR_FORBIDDEN);
		} else { this->_setEnvironmentVars(); }
	}
}

/* Destructor Method */
CGIResponse::~CGIResponse( void ) {};

/* Private Methods */
void	CGIResponse::_setEnvironmentVars( void ) {
	_env["SERVER_PROTOCOL"] = _request.getVersion();
	_env["REQUEST_METHOD"] = _request.getMethod();
	_env["REQUEST_URI"] = _request.getUri();
	_env["SCRIPT_NAME"] = _locationConfig.cgiPath;
	_env["SCRIPT_FILENAME"] = _cgiPath;
	_env["PATH_INFO"] = _getPathInfo(_request.getUri());
	_env["QUERY_STRING"] = _getQueryString(_request.getUri());
	if (_request.getMethod() == POST) {
		std::string contentLength = _request.getHeader("Content-Length");
		std::string contentType = _request.getHeader("Content-Type");

		if (!contentLength.empty()) {
			_env["CONTENT_LENGTH"] = contentLength;
		} else { _env["CONTENT_LENGTH"] = DEFAULT_EMPTY; }

		if (!contentType.empty()) {
			_env["CONTENT_TYPE"] = contentType;
		} else { _env["CONTENT_TYPE"] = "text/plain"; }
	}
	if (_locationConfig.uploadEnabled) {
		_env["UPLOAD_PATH"] = _locationConfig.uploadPath;
	}
}

char	**CGIResponse::_generateEnvp( void ) {
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

std::string CGIResponse::_getContentLength( void ) const {
	std::string contentLength = _request.getHeader("Content-Length");

	if (!contentLength.empty()) { return contentLength; }
	else { return DEFAULT_EMPTY; }
}

std::string	CGIResponse::_getExecutable( const std::string &extension ) {
	if (extension == EXTENSION_PHP) { return PHP_EXECUTABLE; }
	else if (extension == EXTENSION_PY) { return PYTHON_EXECUTABLE; }
	return (DEFAULT_EMPTY);
}

std::string	CGIResponse::_getQueryString( const std::string &uri ) const {
	std::size_t pos = uri.find('?');
	if (pos != std::string::npos) { 
		return uri.substr(pos + 1); 
	} else { return DEFAULT_EMPTY; }
}

std::string	CGIResponse::_getPathInfo( const std::string &uri ) const {
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

void	CGIResponse::_handleCGIError( int code, const std::string &message ) {
	_body.clear();
	_statusCode = CGIUtils::intToString(code);
	_reasonPhrase = message;
	_logger.logError(LOG_ERROR, message, true);
	std::string errorPage = _locationConfig.server->errorPages[_statusCode];
	_logger.logDebug(LOG_INFO, "Error page: " + errorPage, true);
	handleError(_statusCode, errorPage, message, _logger);
}

bool	CGIResponse::_waitChild( pid_t pid, int &status, std::clock_t start ) {
	if (!waitpid(pid, &status, WNOHANG)) {
		while (double(std::clock() - start) / CLOCKS_PER_SEC <= 2.0) {
			if (waitpid(pid, &status, WNOHANG))
				break ;
		}
		if (!waitpid(pid, &status, WNOHANG)) {
			kill(pid, SIGKILL);
			status = TIMEOUT_ERROR;
			return (false);
		}
	}
	return (true);
}

void	CGIResponse::_readReturnBody( int pipefd[2] ) {
	char buffer[4096];
	size_t bytes_read;

	close(pipefd[1]);
	bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
	buffer[bytes_read] = '\0';
	std::string strBuffer(buffer);
	if (!strBuffer.empty()) { _body = strBuffer; }
	handleFileResponse(DEFAULT_EMPTY, _logger);
	_logger.logDebug(LOG_INFO, "CGI response: " + _body, true);
	close(pipefd[0]);
}

/* Public Methods */
void	CGIResponse::executeCGI( void ) {
	if (_statusCode != "200") { return ; }

	std::clock_t start = std::clock();
	char **envp = _generateEnvp();
	char *argv[] = { 
		const_cast<char *>(_cgiExecutable.c_str()), 
		const_cast<char *>(_cgiPath.c_str()), NULL 
	};
	int	pipefd[2];
	int status = 0;

	if (pipe(pipefd) == -1) {
		_handleCGIError(500, ERORR_CREATE_PIPE);
		return ;
	}
	pid_t pid = fork();
	if (pid == -1) {
		_handleCGIError(500, ERROR_CREATE_FORK);
		return ;
	} else if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);

		if (_request.getMethod() == POST) {
			int postPipe[2];
			if (pipe(postPipe) == -1) {
				exit(EXIT_FAILURE);
			}
			dup2(postPipe[0], STDIN_FILENO);
			close(postPipe[0]);

			std::string body = _request.getBody();
			write(postPipe[1], body.c_str(), body.length());
			close(postPipe[1]);
		}
		close(pipefd[1]);

		if (execve(_cgiExecutable.c_str(), argv, envp) == -1) {
			exit(EXIT_FAILURE);
		}
	} else {
		if (_waitChild(pid, status, start)) { _readReturnBody(pipefd); }

		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			_handleCGIError(500, ERROR_CGI_EXECUTION);
		} else {
			if (_request.getMethod() == GET) {
				_statusCode = "200";
				_reasonPhrase = "OK";
			} else if (_request.getMethod() == POST) {
				std::string uploadPath = _locationConfig.uploadPath;
				std::string body = _request.getBody();
				std::string file = CGIUtils::extractFileName(body);

				if (ConfigUtils::fileExists(uploadPath + "/" + file)) {
					_statusCode = "201";
					_reasonPhrase = "Created";
				} else {
					_statusCode = "200";
					_reasonPhrase = "OK";
				}
			}
		}
	}
	CGIUtils::deleteEnvp(envp);
}
