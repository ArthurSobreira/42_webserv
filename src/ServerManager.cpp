#include "ServerManager.hpp"

/* Constructor Method */
ServerManager::ServerManager(const std::string &configFilePath)
	: _epollManager(), _config(configFilePath, logger) {
	if (!_initializeServers()) {
		throw std::runtime_error("Failed to initialize servers");
	}
}

/* Destructor Method */
ServerManager::~ServerManager( void ) {
	for (std::vector<Server *>::iterator it = _servers.begin(); 
	it != _servers.end(); ++it) {
		delete *it;
	}
}

/* Public Method */
void ServerManager::run( void ) {
	while (!stop) {
		_handleEvents();
	}
}

/* Private Methods */
bool ServerManager::_initializeServers( void ) {
	std::vector<ServerConfigs> serverConfigs = _config.getServers();

	for (std::vector<ServerConfigs>::iterator it = serverConfigs.begin(); 
		it != serverConfigs.end(); ++it) {
		Server *server = new Server(*it, _epollManager);
		if (!server->initialize())
		{
			delete server;
			return false;
		}
		_servers.push_back(server);
		_fds.addFdToServer(server->getServerSocket());
		logger.logDebug(LOG_INFO, "Server initialized on " + 
			it->host + ":" + CGIUtils::intToString(it->port), true);
	}
	return true;
}

void ServerManager::_handleEvents( void ) {
	epoll_event events[MAX_EVENTS];
	int nfds = epoll_wait(_epollManager.getEpollFD(), events, MAX_EVENTS, -1);
	if (nfds == -1) {
		logger.logError(LOG_ERROR, "Error on epoll_wait", true);
		return;
	}
	for (int i = 0; i < nfds; i++) {
		if (_fds.isFdInServer(events[i].data.fd)) {
			if (events[i].events & EPOLLIN) {
				logger.logAccess(LOG_INFO, "Accepting connection on server socket: " +
					CGIUtils::intToString(events[i].data.fd), true);
				_acceptConnection(events[i].data.fd);
			}
		}
		else {
			if (events[i].events & EPOLLIN) {
				logger.logDebug(LOG_DEBUG, "Handling read on client socket: " + 
					CGIUtils::intToString(events[i].data.fd));
				_handleRead(events[i].data.fd);
			}
			if (events[i].events & EPOLLOUT) {
				logger.logDebug(LOG_DEBUG, "Handling write on client socket: " + 
					CGIUtils::intToString(events[i].data.fd));
				_handleWrite(events[i].data.fd);
			}
		}
	}
}

void ServerManager::_acceptConnection( int serverSocket ) {
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket == -1) {
		logger.logError(LOG_ERROR, "Error accepting connection", true);
		return;
	}
	_epollManager.addToEpoll(clientSocket, EPOLLIN);
	_requestMap[clientSocket] = "";
	_responseMap[clientSocket] = "";
	_clientServerMap[clientSocket] = serverSocket;
	_connectionMap[clientSocket] = true;
	logger.logAccess(LOG_INFO, "Accepted connection on IP:" + 
		inetNtop(clientAddr.sin_addr.s_addr) + " Port:" +
		CGIUtils::intToString(ntohs(clientAddr.sin_port)), true);
}

void ServerManager::_handleRead( int clientSocket ) {
	char buffer[65535];
	bzero(buffer, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead == -1) {
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	else if (bytesRead == 0) {
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	else {
		std::string lenfVerif = std::string(buffer, bytesRead);
		if (lenfVerif.size() < 65535) {
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			_requestMap[clientSocket] += lenfVerif;
			return;
		}
	}
	_requestMap[clientSocket] += std::string(buffer, bytesRead);
	logger.logDebug(LOG_DEBUG, "Request received: " + _requestMap[clientSocket]);
}

void ServerManager::_handleWrite( int clientSocket ) {
	if (_responseMap[clientSocket] == "") {
		if (!_verifyContentLength(clientSocket, _requestMap[clientSocket]))
			return;
		Request request(_requestMap[clientSocket]);
		for (std::vector<Server *>::iterator it = _servers.begin(); 
			it != _servers.end(); ++it) {
			if ((*it)->getServerSocket() == _clientServerMap[clientSocket])
			{
				logger.logDebug(LOG_INFO, "Handling response for client socket: " + 
					CGIUtils::intToString(clientSocket), true);
				_handleResponse(request, (*it)->getConfig(), clientSocket);
				_requestMap[clientSocket] = "";
				break;
			}
		}
	} else {
		int bytesWritten = send(clientSocket, _responseMap[clientSocket].c_str(), 
			_responseMap[clientSocket].size(), 0);

		if (bytesWritten <= 0) {
			logger.logError(LOG_ERROR, "Error sending response", true);
			_epollManager.removeFromEpoll(clientSocket);
			close(clientSocket);
			return;
		}
		else if (bytesWritten < (int)_responseMap[clientSocket].size()) {
			logger.logDebug(LOG_INFO, "Partial response sent, remaining: " + 
				CGIUtils::intToString(_responseMap[clientSocket].size()) + " bytes", true);
			_responseMap[clientSocket] = _responseMap[clientSocket].substr(bytesWritten);
		}
		else if (bytesWritten == (int)_responseMap[clientSocket].size())
		{
			logger.logDebug(LOG_INFO, "Full response sent", true);
			_responseMap[clientSocket] = DEFAULT_EMPTY;
			if (_connectionMap[clientSocket]) {
				logger.logDebug(LOG_INFO, "Connection closed after sending response", true);
				_closeConnection(clientSocket);
				return;
			}
			_epollManager.modifyEpoll(clientSocket, EPOLLIN);
			logger.logDebug(LOG_DEBUG, "Modified epoll to EPOLLIN for client socket: " + 
				CGIUtils::intToString(clientSocket));
		}
	}
}

bool ServerManager::_verifyContentLength( int clientSocket, std::string &buffer ) {
	std::string contentLengthHeader = "Content-Length: ";
	size_t pos = buffer.find(contentLengthHeader);

	if (pos != std::string::npos) {
		size_t endOfHeader = buffer.find("\r\n", pos);
		std::string contentLengthStr = buffer.substr(pos + contentLengthHeader.size(),
			endOfHeader - (pos + contentLengthHeader.size()));
		std::stringstream ss;

		ss << contentLengthStr;
		size_t contentLength;
		ss >> contentLength;
		size_t endOfHeaders = buffer.find("\r\n\r\n");
		if (endOfHeaders == std::string::npos) {
			_epollManager.modifyEpoll(clientSocket, EPOLLIN);
			return false;
		}
		if (buffer.size() - endOfHeaders - 4 >= contentLength) {
			return true;
		} else {
			_epollManager.modifyEpoll(clientSocket, EPOLLIN);
			return false;
		}
	}
	return true;
}

void ServerManager::_handleResponse( Request &request, ServerConfigs &server, 
	int clientSocket ) {
	std::string status = request.validateRequest(_config, server);
	_connectionMap[clientSocket] = request.connectionClose();
	if (status != DEFAULT_EMPTY) {
		_handleError(clientSocket, server.errorPages[status], status);
		return;
	}
	if (request.isCGI()) {
		logger.logDebug(LOG_INFO, "CGI request detected", true);
		CGIResponse cgi(request, request.getLocation());

		cgi.executeCGI();
		_responseMap[clientSocket] = cgi.generateResponse();
		return;
	}

	switch (request.getMethod())
	{
		case GET:
		{
			logger.logDebug(LOG_INFO, "GET request detected", true);
			GetResponse getResponse(request.getUri());

			getResponse.prepareResponse(request.getLocation());
			_responseMap[clientSocket] = getResponse.generateResponse();
			break;
		}
		case POST:
		{
			logger.logDebug(LOG_INFO, "POST request detected", true);
			PostResponse postResponse(request.getUri(), request.getBody(), server, 
				request.getLocation(), request.getHeaders());

			postResponse.prepareResponse();
			_responseMap[clientSocket] = postResponse.generateResponse();
			break;
		}
		case DELETE:
		{
			logger.logDebug(LOG_INFO, "DELETE request detected", true);
			DeleteResponse deleteResponse(request.getUri(), server);

			deleteResponse.prepareResponse();
			_responseMap[clientSocket] = deleteResponse.generateResponse();
			break;
		}
		default:
			break;
	}
}

void ServerManager::_closeConnection( int clientSocket ) {
	_epollManager.removeFromEpoll(clientSocket);
	_requestMap.erase(clientSocket);
	_responseMap.erase(clientSocket);
	_clientServerMap.erase(clientSocket);
	_connectionMap.erase(clientSocket);
	close(clientSocket);
}

void ServerManager::_handleError( int clientSocket, const std::string &errorPage, 
	const std::string &status ) {
	std::string response = "HTTP/1.1 " + status + " " + errorPage + "\r\n";
	std::stringstream ss;
	std::string body = readFile(errorPage);
	ss << body.size();
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += body;
	_responseMap[clientSocket] = response;
	logger.logError(LOG_ERROR, "Error: " + status, true);	
}
