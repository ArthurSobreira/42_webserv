#include "CGIResponse.hpp"

/* Constructor Method */
CGIResponse::CGIResponse( const Request &request, const LocationConfigs &location ) 
	: Response(), _request(request), _location(location) {
	if (_location.cgiEnabled) {
		_cgiPath = location.root + "/" + location.cgiPath;
		_cgiExecutable = _getExecutable(location.cgiExtension);

		if (access(_cgiPath.c_str(), X_OK) == -1) {
			_handleCGIError(403, ERROR_FORBIDDEN);
		} else if (!CGIUtils::methodIsOnLocation(_location, 
			_request.getMethod()) || _request.getMethod() == DELETE) {
			_handleCGIError(405, ERROR_METHOD_NOT_ALLOWED);
		} else if (CGIUtils::isUploadRequest(_request) && 
			!_location.uploadEnabled) {
			_handleCGIError(403, ERROR_FORBIDDEN);
		} else if (_request.getMethod() == POST && 
			!_request.getHeader("Content-Length").empty() && 
			_request.getBody().size() > 
			_location.server->limitBodySize) {
			_handleCGIError(413, ERROR_TOO_LARGE);
		} else { this->_setEnvironmentVars(); }
	}
}

/* Destructor Method */
CGIResponse::~CGIResponse( void ) {};

/* Private Methods */
void	CGIResponse::_setEnvironmentVars( void ) {
	std::string method = getStringMethod(_request.getMethod());
	_env["SERVER_PROTOCOL"] = _request.getVersion();
	_env["QUERY_STRING"] = _request.getQueryString();
	_env["REQUEST_URI"] = _getCompleteUri();
	_env["REQUEST_METHOD"] = method;
	_env["SCRIPT_FILENAME"] = _cgiPath;
	_env["SCRIPT_NAME"] = _location.cgiPath;
	_env["PATH_INFO"] = _getPathInfo(_request.getUri());
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
	if (_location.uploadEnabled) {
		_env["UPLOAD_PATH"] = _location.uploadPath;
	}
}

char	**CGIResponse::_generateEnvp( void ) {
	char **envp = new char*[_env.size() + 1];
	int index = 0;

	for (stringMap::iterator it = _env.begin(); 
		it != _env.end(); ++it) {
		std::string envVar = it->first + "=" + it->second;
		envp[index] = new char[envVar.length() + 1];
		std::strcpy(envp[index], envVar.c_str());
		index++;
	}
	envp[index] = NULL;
	return (envp);
}

std::string	CGIResponse::_getCompleteUri( void ) const {
	std::string uri = _request.getUri();
	std::string queryString = _request.getQueryString();

	if (!queryString.empty()) { return (uri + "?" + queryString); }
	else { return uri; }
}

std::string	CGIResponse::_getPathInfo( const std::string &uri ) const {
	std::string scriptName = _location.cgiPath;
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

std::string	CGIResponse::_getExecutable( const std::string &extension ) {
	if (extension == EXTENSION_PHP) { return PHP_EXECUTABLE; }
	else if (extension == EXTENSION_PY) { return PYTHON_EXECUTABLE; }
	return (DEFAULT_EMPTY);
}

void	CGIResponse::_handleCGIError( int code, const std::string &message ) {
	_body.clear();
	_statusCode = intToString(code);
	_reasonPhrase = message;
	std::string errorPage = _location.server->errorPages[_statusCode];
	handleError(_statusCode, errorPage, message);
}

bool	CGIResponse::_waitChild( pid_t pid, int &status, std::clock_t start ) {
	if (!waitpid(pid, &status, WNOHANG)) {
		while (double(std::clock() - start) / CLOCKS_PER_SEC <= 2.0) {
			if (waitpid(pid, &status, WNOHANG))
				break ;
		}
		if (!waitpid(pid, &status, WNOHANG)) {
			kill(pid, SIGKILL);
			logger.logError(LOG_ERROR, ERROR_CGI_TIMEOUT, true);
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
	handleFileResponse(DEFAULT_EMPTY);
	close(pipefd[0]);
}

void	CGIResponse::_sendBodyToCGI( const std::string &body ) {
	if (body.empty()) { return; }

	char tempFileName[] = CGI_TEMP_FILE;
	int tempFileFd = mkstemp(tempFileName);
	if (tempFileFd == -1) { exit(EXIT_FAILURE); }

	const size_t chunkSize = 4096;
	size_t bytesRemaining = body.length();
	size_t bytesSent = 0;
	while (bytesRemaining > 0) {
		size_t toWrite = std::min(chunkSize, bytesRemaining);
		ssize_t bytesWritten = write(tempFileFd, body.c_str() + 
			bytesSent, toWrite);

		if (bytesWritten == -1) {
			close(tempFileFd);
			unlink(tempFileName);
			exit(EXIT_FAILURE);
		}
		bytesSent += bytesWritten;
		bytesRemaining -= bytesWritten;
	}
	lseek(tempFileFd, 0, SEEK_SET);
	dup2(tempFileFd, STDIN_FILENO);
	close(tempFileFd);
	unlink(tempFileName);
}

/* Public Method */
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
		close(pipefd[1]);

		if (_request.getMethod() == POST) {
			_sendBodyToCGI(_request.getBody());
		}

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
				std::string uploadPath = _location.uploadPath;
				std::string body = _request.getBody();
				std::string file = CGIUtils::extractFileName(body);

				if (CGIUtils::isUploadRequest(_request)) {
					std::string	fullFilePath = uploadPath + "/" + file;
					if (ConfigUtils::fileExists(fullFilePath)) {
						logger.logDebug(LOG_INFO, "File Created: " + 
							fullFilePath, true);
						_statusCode = "201";
						_reasonPhrase = "Created";
					} else {
						logger.logError(LOG_ERROR, "Error creating file: " +
							fullFilePath, true);
						_handleCGIError(500, ERROR_CGI_EXECUTION);
					}
				} else {
					_statusCode = "200";
					_reasonPhrase = "OK";
				}
			}
		}
	}
	CGIUtils::deleteEnvp(envp);
}
