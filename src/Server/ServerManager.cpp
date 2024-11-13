#include "ServerManager.hpp"

/* Struct ClientData Constructor */
ClientData::ClientData( const std::string &req, const std::string &res, 
	bool conn, bool compReq, size_t contentLen, size_t bRead ) {
	request = req;
	response = res;
	connection = conn;
	completeRequest = compReq;
	contentLength = contentLen;
	bytesRead = bRead;
}

/* Constructor Method */
ServerManager::ServerManager( const std::string &configFilePath )
	: _epollManager(), _config(configFilePath) {
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
	std::vector<ServerConfigs> serverConfigs = _config.getServers();
	for (std::vector<ServerConfigs>::iterator it = serverConfigs.begin();
		it != serverConfigs.end(); ++it) {
		for (std::vector<LocationConfigs>::iterator it2 = it->locations.begin();
			it2 != it->locations.end(); ++it2) {
			delete it2->server;
		}
	}
}

/* Public Method */
void	ServerManager::run( void ) {
	while (!stop) {
		_handleEvents();
	}
}

/* Private Methods */
bool	ServerManager::_initializeServers( void ) {
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

void	ServerManager::_handleEvents( void ) {
	epoll_event events[MAX_EVENTS];
	int nfds = epoll_wait(_epollManager.getEpollFD(), events, MAX_EVENTS, -1);
	if (nfds == -1) {
		logger.logError(LOG_ERROR, "Error on epoll_wait");
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

void	ServerManager::_acceptConnection( int serverSocket ) {
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket == -1) {
		logger.logError(LOG_ERROR, "Error accepting connection", true);
		return;
	}
	_epollManager.addToEpoll(clientSocket, EPOLLIN);
	_clientDataMap[clientSocket] = ClientData("", "", false, true, 0, 0);
	_clientServerMap[clientSocket] = serverSocket;
	logger.logAccess(LOG_INFO, "Accepted connection on IP: " + 
		inetNtop(clientAddr.sin_addr.s_addr) + " and Port: " +
		CGIUtils::intToString(ntohs(clientAddr.sin_port)), true);
}

void	ServerManager::_handleRead( int clientSocket ) {
	char buffer[65535];
	memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	_clientDataMap[clientSocket].bytesRead += bytesRead;
	if (bytesRead <= 0) {
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	std::string data(buffer, bytesRead);
	if (_clientDataMap[clientSocket].completeRequest) {
		_clientDataMap[clientSocket].request += data;
	}
	if (_clientDataMap[clientSocket].contentLength == 0) {
		_getContentLength(clientSocket, data);
		if (_clientDataMap[clientSocket].contentLength >= MAX_BODY_SIZE) {
			_clientDataMap[clientSocket].completeRequest = false;
			return;
		}
	}
	if (_clientDataMap[clientSocket].contentLength > 0) {
		if (_clientDataMap[clientSocket].bytesRead >= 
			_clientDataMap[clientSocket].contentLength) {
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			logger.logDebug(LOG_DEBUG, "Complete request case 1");
			return;
		}
	}
	else {
		if (data.find("\r\n\r\n") != std::string::npos) {
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			logger.logDebug(LOG_DEBUG, "Complete request case 2");
			return;
		}
	}
	std::string size = CGIUtils::intToString(data.size());
	logger.logDebug(LOG_DEBUG, "Request received: " + data);
	logger.logDebug(LOG_DEBUG, "Read: " + size + " bytes");
}

void	ServerManager::_handleWrite( int clientSocket ) {
	if (_clientDataMap[clientSocket].response == DEFAULT_EMPTY) {
		Request request(_clientDataMap[clientSocket].request, 
			_clientDataMap[clientSocket].completeRequest);
		for (std::vector<Server *>::iterator it = _servers.begin();
			it != _servers.end(); ++it) {
			if ((*it)->getServerSocket() == _clientServerMap[clientSocket])
			{
				logger.logDebug(LOG_INFO, "Handling response for client socket: " + 
					CGIUtils::intToString(clientSocket), true);
				_handleResponse(request, (*it)->getConfig(), clientSocket);
				_clientDataMap[clientSocket].request = DEFAULT_EMPTY;
				break;
			}
		}
	}
	else {
		int bytesWritten = send(clientSocket, _clientDataMap[clientSocket].response.c_str(), 
			_clientDataMap[clientSocket].response.size(), 0);

		if (bytesWritten <= 0) {
			logger.logError(LOG_ERROR, "Connection closed due to send error", true);
			_epollManager.removeFromEpoll(clientSocket);
			close(clientSocket);
			return;
		}
		else if (bytesWritten < (int)_clientDataMap[clientSocket].response.size()) {
			logger.logDebug(LOG_INFO, "Partial response sent, remaining: " + 
				CGIUtils::intToString(_clientDataMap[clientSocket].response.size()) + " bytes", true);
			_clientDataMap[clientSocket].response = _clientDataMap[clientSocket].response.substr(bytesWritten);
		}
		else if (bytesWritten == (int)_clientDataMap[clientSocket].response.size()) {
			logger.logDebug(LOG_INFO, "Full response sent to client socket: " + 
				CGIUtils::intToString(clientSocket), true);
			_restartStruct(_clientDataMap[clientSocket]);
			if (_clientDataMap[clientSocket].connection) {
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

void	ServerManager::_handleResponse( Request &request, ServerConfigs &server,
	int clientSocket ) {
	std::string status = request.validateRequest(_config, server, 
		_clientDataMap[clientSocket].completeRequest);
	_clientDataMap[clientSocket].connection = request.connectionClose();
	if (status != DEFAULT_EMPTY) {
		_handleError(clientSocket, server.errorPages[status], status,
			getErrorMessage(status));
		return;
	}
	if (request.isRedirect()) {
		_clientDataMap[clientSocket].response = 
			_handleRedirect(request.getLocation().redirect);
		return;
	}
	if (request.isCGI()) {
		logger.logDebug(LOG_INFO, "CGI request detected", true);
		CGIResponse cgi(request, request.getLocation());

		cgi.executeCGI();
		_clientDataMap[clientSocket].response = cgi.generateResponse();
		return;
	}

	switch (request.getMethod())
	{
		case GET:
		{
			GetResponse getResponse(request.getUri());
			getResponse.prepareResponse(request.getLocation());
			_clientDataMap[clientSocket].response = getResponse.generateResponse();
			break;
		}
		case POST:
		{
			PostResponse postResponse(request.getUri(), request.getBody(),
				request.getHeaders(), request.getLocation());
			postResponse.prepareResponse();
			_clientDataMap[clientSocket].response = postResponse.generateResponse();
			break;
		}
		case DELETE:
		{
			DeleteResponse deleteResponse(request.getUri(), request.getLocation());
			deleteResponse.prepareResponse();
			_clientDataMap[clientSocket].response = deleteResponse.generateResponse();
			break;
		}
		default:
			break;
	}
}

void	ServerManager::_getContentLength( int clientSocket, std::string &buffer ) {
	std::string contentLengthHeader = "Content-Length: ";
	size_t pos = buffer.find(contentLengthHeader);

	if (pos != std::string::npos) {
		size_t endOfHeader = buffer.find("\r\n", pos);
		if (endOfHeader != std::string::npos) {
			std::string contentLengthStr = buffer.substr(pos + contentLengthHeader.size(),
				endOfHeader - (pos + contentLengthHeader.size()));
			std::stringstream ss;

			ss << contentLengthStr;
			size_t contentLength;
			ss >> contentLength;
			_clientDataMap[clientSocket].contentLength = contentLength;
			logger.logDebug(LOG_DEBUG, "Extracted Content-Length: " + 
				CGIUtils::intToString(contentLength));
		} else {
			logger.logError(LOG_ERROR, "End of Content-Length header not found", true);
		}
	} else {
		logger.logDebug(LOG_DEBUG, "Content-Length header not found");
	}
}

std::string ServerManager::_handleRedirect( const std::string &location ) {
	std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
	response += "Location: " + location + "\r\n";
	response += "Content-Length: 0\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	return response;
}

void	ServerManager::_restartStruct( ClientData &data ) {
	data.request = DEFAULT_EMPTY;
	data.response = DEFAULT_EMPTY;
	data.connection = false;
	data.completeRequest = true;
	data.contentLength = 0;
	data.bytesRead = 0;
}

void	ServerManager::_closeConnection( int clientSocket ) {
	_epollManager.removeFromEpoll(clientSocket);
	_clientDataMap.erase(clientSocket);
	close(clientSocket);
}

void	ServerManager::_handleError( int clientSocket, const std::string &errorPage,
	const std::string &status, const std::string &error_message ) {
	std::string response = "HTTP/1.1 " + status + " " + errorPage + "\r\n";
	std::stringstream ss;
	std::string body = readFile(errorPage);
	ss << body.size();
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += body;
	_clientDataMap[clientSocket].response = response;
	logger.logError(LOG_ERROR, "Error: " + error_message, true);	
}
