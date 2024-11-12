#include "ServerManager.hpp"

ServerManager::ServerManager(const std::string &configFilePath)
	: _logger(new Logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE)),
	  _epollManager(*_logger),
	  _config(configFilePath, *_logger)
{
	if (!initializeServers())
	{
		throw std::runtime_error("Failed to initialize servers");
	}
}

ServerManager::~ServerManager()
{
	delete _logger;
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		delete *it;
	}
}

bool ServerManager::initializeServers()
{
	std::vector<ServerConfigs> serverConfigs = _config.getServers();

	for (std::vector<ServerConfigs>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	{
		// Alocar dinamicamente as instâncias de Server e armazená-las no vetor _servers
		Server *server = new Server(*it, *_logger, _epollManager);
		if (!server->initialize())
		{
			delete server;
			return false;
		}
		std::cout << server->getServerSocket() << std::endl;
		_servers.push_back(server);
		_fds.addFdToServer(server->getServerSocket());
		std::cout << "Server initialized and added to epoll: " << server->getServerSocket() << std::endl;
	}

	return true;
}

void ServerManager::run()
{
	while (!stop)
	{
		handleEvents();
	}
}

void ServerManager::acceptConnection(int serverSocket)
{
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket == -1)
	{
		_logger->logError(LOG_ERROR, "Error accepting connection", true);
		return;
	}
	_epollManager.addToEpoll(clientSocket, EPOLLIN);
	_clientDataMap[clientSocket] = clientData("", "", false, true, 0);
	_clientServerMap[clientSocket] = serverSocket;

	// _fds.addFdToServer(clientSocket);
}

void ServerManager::handleEvents()
{
	epoll_event events[MAX_EVENTS];
	int nfds = epoll_wait(_epollManager.getEpollFD(), events, MAX_EVENTS, -1);
	if (nfds == -1)
	{
		_logger->logError(LOG_ERROR, "Error on epoll_wait", true);
		return;
	}
	for (int i = 0; i < nfds; i++)
	{
		// std::cout << "Event received on fd: " << events[i].data.fd << std::endl;
		if (_fds.isFdInServer(events[i].data.fd))
		{
			if (events[i].events & EPOLLIN)
			{
				// std::cout << "EPOLLIN event on server socket: " << events[i].data.fd << std::endl;
				acceptConnection(events[i].data.fd);
			}
		}
		else
		{
			if (events[i].events & EPOLLIN)
			{
				// std::cout << "EPOLLIN event on client socket: " << events[i].data.fd << std::endl;
				handleRead(events[i].data.fd);
			}
			if (events[i].events & EPOLLOUT)
			{
				// std::cout << "EPOLLOUT event on client socket: " << events[i].data.fd << std::endl;
				handleWrite(events[i].data.fd);
			}
		}
	}
}

void ServerManager::getContentLength(int clientSocket, std::string &buffer)
{
	std::string contentLengthHeader = "Content-Length: ";
	size_t pos = buffer.find(contentLengthHeader);

	if (pos != std::string::npos)
	{
		size_t endOfHeader = buffer.find("\r\n", pos);
		if (endOfHeader != std::string::npos)
		{
			std::string contentLengthStr = buffer.substr(pos + contentLengthHeader.size(),
														 endOfHeader - (pos + contentLengthHeader.size()));
			std::stringstream ss;
			ss << contentLengthStr;
			size_t contentLength;
			ss >> contentLength;
			_clientDataMap[clientSocket].contentLength = contentLength;
			std::cout << "Extracted Content-Length: " << contentLength << std::endl;
		}
		else
		{
			std::cerr << "Error: End of Content-Length header not found" << std::endl;
		}
	}
	else
	{
		std::cerr << "Error: Content-Length header not found" << std::endl;
	}
}

void ServerManager::handleRead(int clientSocket)
{
	static int counter = 0;
	std::cout << "Handling read on socket: " << clientSocket << std::endl;

	char buffer[65535];
	memset(buffer, 0, sizeof(buffer));

	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	_clientDataMap[clientSocket].bytesRead += bytesRead;
	std::cout << "Loop número: " << counter++ << std::endl;
	if (bytesRead <= 0)
	{
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	std::string data(buffer, bytesRead);
	if (_clientDataMap[clientSocket].completeRequest)
		_clientDataMap[clientSocket].request += data;
		
	if (_clientDataMap[clientSocket].contentLength == 0)
	{
		getContentLength(clientSocket, data);
		std::cout << "Content-Length: " << _clientDataMap[clientSocket].contentLength << std::endl;
		std::cout << "MAX_BODY_SIZE: " << MAX_BODY_SIZE << std::endl;
		if (_clientDataMap[clientSocket].contentLength >= MAX_BODY_SIZE)
		{
			_clientDataMap[clientSocket].completeRequest = false;
			return;
		}
	}
	if (_clientDataMap[clientSocket].contentLength > 0)
	{
		if (_clientDataMap[clientSocket].bytesRead >= _clientDataMap[clientSocket].contentLength)
		{
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			std::cout << "Complete request1" << std::endl;
			return;
		}
	}
	else
	{
		if (data.find("\r\n\r\n") != std::string::npos)
		{
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			std::cout << "Complete request2" << std::endl;
			return;
		}
	}
	std::cout << "Read: " << data.size() << " bytes" << std::endl;
}

void ServerManager::handleError(int clientSocket, Logger *logger, const std::string &errorPage, const std::string &status)
{
	std::string response = "HTTP/1.1 " + status + " " + errorPage + "\r\n";
	std::stringstream ss;
	std::string body = readFile(errorPage);
	ss << body.size();
	response += "Content-Length: " + ss.str() + "\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += body;
	_clientDataMap[clientSocket].response = response;
	logger->logError(LOG_ERROR, "Error: " + status, true);	
}

void ServerManager::handleResponse(Request &request, ServerConfigs &server, int clientSocket)
{
	std::string status = request.validateRequest(_config, server, _clientDataMap[clientSocket].completeRequest);
	std::cout << "vazio é o normal [" << status << "]" << std::endl;
	_clientDataMap[clientSocket].connection = request.connectionClose();
	if (status != "")
	{
		handleError(clientSocket, _logger, server.errorPages[status], status);
		return;
	}
	if (request.isCGI())
	{
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
		PostResponse postResponse(request.getUri(), request.getBody(), server, request.getLocation(), request.getHeaders());
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

void ServerManager::closeConnection(int clientSocket)
{
	_epollManager.removeFromEpoll(clientSocket);
	_clientDataMap.erase(clientSocket);
	close(clientSocket);
}

void restartStruct(clientData &data)
{
	data.request = "";
	data.response = "";
	data.connection = false;
	data.completeRequest = true;
	data.contentLength = 0;
	data.bytesRead = 0;
}

void ServerManager::handleWrite(int clientSocket)
{
	if (_clientDataMap[clientSocket].response == "")
	{
			Request request(_clientDataMap[clientSocket].request, _clientDataMap[clientSocket].completeRequest);
			for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
			{
				if ((*it)->getServerSocket() == _clientServerMap[clientSocket])
				{
					handleResponse(request, (*it)->getConfig(), clientSocket);
					_clientDataMap[clientSocket].request = "";
					break;
				}
			}
	}
	else
	{
		int bytesWritten = send(clientSocket, _clientDataMap[clientSocket].response.c_str(), _clientDataMap[clientSocket].response.size(), 0);
		if (bytesWritten <= 0)
		{
			_epollManager.removeFromEpoll(clientSocket);
			close(clientSocket);
			std::cout << "Connection closed due to send error" << std::endl;
			return;
		}
		else if (bytesWritten < (int)_clientDataMap[clientSocket].response.size())
		{
			_clientDataMap[clientSocket].response = _clientDataMap[clientSocket].response.substr(bytesWritten);
			std::cout << "Partial response sent, remaining: " << _clientDataMap[clientSocket].response.size() << " bytes" << std::endl;
		}
		else if (bytesWritten == (int)_clientDataMap[clientSocket].response.size())
		{
			restartStruct(_clientDataMap[clientSocket]);
			std::cout << "Full response sent" << std::endl;
			if (_clientDataMap[clientSocket].connection)
			{
				closeConnection(clientSocket);
				std::cout << "Connection closed after sending response" << std::endl;
				return;
			}
			_epollManager.modifyEpoll(clientSocket, EPOLLIN);
			std::cout << "Modified epoll to EPOLLIN for client socket: " << clientSocket << std::endl;
		}
	}
}