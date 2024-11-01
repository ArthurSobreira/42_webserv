#include "ServerManager.hpp"
#include "Globals.hpp"
#include "Includes.hpp"
#include "CGIResponse.hpp"

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
	_requestMap[clientSocket] = "";
	_responseMap[clientSocket] = "";
	_clientServerMap[clientSocket] = serverSocket;
	_connectionMap[clientSocket] = true;
	_boundaryMap[clientSocket] = false;
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

void ServerManager::handleRead(int clientSocket)
{
	static int counter = 0;
	std::cout << "Handling read on socket: " << clientSocket << std::endl;
	char buffer[65535];
	bzero(buffer, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "loop numero: " << counter++ << std::endl;
	if (bytesRead == -1)
	{
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	else if (bytesRead == 0)
	{
		_epollManager.removeFromEpoll(clientSocket);
		close(clientSocket);
		return;
	}
	else
	{
		std::string lenfVerif = std::string(buffer, bytesRead);
		if (lenfVerif.size() < 65535)
		{
			lenfVerif[lenfVerif.size() - 1] = '\0';
			if (lenfVerif.find("boundary=") != std::string::npos)
			{
				std::cout << "achei o boundary" << std::endl;
				_boundaryMap[clientSocket] = true;
			}
			_epollManager.modifyEpoll(clientSocket, EPOLLOUT);
			_requestMap[clientSocket] += lenfVerif;
			std::cout << "entrei nesse if: " << lenfVerif.size() << std::endl;
			return;
		}
	}

	std::cout << "debbug 02" << std::endl;
	_requestMap[clientSocket] += std::string(buffer, bytesRead);
	// std::cout << "Received data: " << std::string(buffer, bytesRead) << std::endl;
	std::cout << "read: success" << std::endl;
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
	_responseMap[clientSocket] = response;
	logger->logError(LOG_ERROR, "Error: " + status, true);	
}
void ServerManager::handleResponse(Request &request, ServerConfigs &server, int clientSocket)
{
	std::string status = request.validateRequest(_config, server);
	std::cout << "vazio é o normal [" << status << "]" << std::endl;
	if (status != "")
	{
		handleError(clientSocket, _logger, server.errorPages[status], status);
		return;
	}
	if (request.isCGI())
	{
		CGIResponse cgi(request, request.getLocation());

		cgi.executeCGI();
		_responseMap[clientSocket] = cgi.generateResponse();
		// _logger->logDebug(LOG_DEBUG, "CGI response: [" + _responseMap[clientSocket] + "]", true);
		return;
	}

	switch (request.getMethod())
	{
	case GET:
	{
		std::cout << "entrou no get" << std::endl;
		GetResponse getResponse(request.getUri());
		getResponse.prepareResponse(request.getLocation(), server);
		_responseMap[clientSocket] = getResponse.generateResponse();
		break;
	}
	case POST:
	{
		// PostResponse postResponse(clientSocket, *_logger, request.getUri());
		// postResponse.prepareResponse();
		// postResponse.sendResponse();
		handleError(clientSocket, _logger, server.errorPages["999"], "999");
		break;
	}
	case DELETE:
	{
		// DeleteResponse deleteResponse(clientSocket, *_logger, request.getUri());
		// deleteResponse.prepareResponse();
		// deleteResponse.sendResponse();
		break;
	}
	default:
		break;
	}
}

void ServerManager::closeConnection(int clientSocket)
{
	_epollManager.removeFromEpoll(clientSocket);
	_requestMap.erase(clientSocket);
	_responseMap.erase(clientSocket);
	close(clientSocket);
}

void ServerManager::handleWrite(int clientSocket)
{
	if (_responseMap[clientSocket] == "")
	{
		if (_boundaryMap[clientSocket]){
			std::cout << "entrei no boundary" << std::endl;
			_epollManager.modifyEpoll(clientSocket, EPOLLIN);
			_boundaryMap[clientSocket] = false;
			return;
		}
		Request request(_requestMap[clientSocket]);
		for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
		{
			if ((*it)->getServerSocket() == _clientServerMap[clientSocket])
			{
				handleResponse(request, (*it)->getConfig(), clientSocket);
				_requestMap[clientSocket] = "";
				_connectionMap[clientSocket] = request.connectionClose();
				break;
			}
		}
	}
	else{
		int bytesWritten = send(clientSocket, _responseMap[clientSocket].c_str(), _responseMap[clientSocket].size(), 0);
		if (bytesWritten <= 0)
		{
			_epollManager.removeFromEpoll(clientSocket);
			close(clientSocket);
			return;
		}
		else if (bytesWritten < (int)_responseMap[clientSocket].size())
		{
			_responseMap[clientSocket] = _responseMap[clientSocket].substr(bytesWritten);
		}
		else if (bytesWritten == (int)_responseMap[clientSocket].size())
		{
			_responseMap[clientSocket] = "";
			if (_connectionMap[clientSocket])
			{
				closeConnection(clientSocket);
				return;
			}
			_epollManager.modifyEpoll(clientSocket, EPOLLIN);
		}
	}
}
