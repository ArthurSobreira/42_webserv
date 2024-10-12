#include "Includes.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Server.hpp"
#include "EpollManager.hpp"
#include "CGI.hpp"

void handleNewConnection(int server_sockfd, int epoll_fd, Logger &logger)
{
	sockaddrIn client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_sockfd = accept(server_sockfd, (sockAddr *)&client_addr, &client_len);
	if (client_sockfd == -1)
	{
		logger.logError(LOG_ERROR, "Error accepting new connection");
		return;
	}
	int flags = fcntl(client_sockfd, F_GETFL, 0);
	fcntl(client_sockfd, F_SETFL, flags | O_NONBLOCK);
	epollEvent ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev) == -1)
	{
		logger.logError(LOG_ERROR, "Failed to add client socket to epoll");
		close(client_sockfd);
	}
	else
		logger.logDebug(LOG_DEBUG, "New client connection accepted");
	getConfig().setSocketServerMap(client_sockfd, server_sockfd);
}

bool readClientData(int client_sockfd, char *buffer, std::string &fullRequest, Logger &logger)
{
	int n = read(client_sockfd, buffer, sizeof(buffer));

	if (n == -1)
	{
		logger.logError(LOG_ERROR, "Error reading from client socket");
		return false;
	}
	else if (n == 0)
	{
		logger.logDebug(LOG_DEBUG, "Client disconnected");
		return false;
	}

	fullRequest.append(buffer, n);
	return true;
}

bool processRequest(Request &request, const std::string &fullRequest, Logger &logger)
{
	if (!request.parseRequest(fullRequest))
	{
		logger.logError(LOG_ERROR, "Invalid HTTP request", true);
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Request processed",true);
	logger.logDebug(LOG_DEBUG, "Method: " + request.getMethod() + ", URI: " + request.getUri() + ", HTTP Version: " + request.getHttpVersion(), true);
	logger.logDebug(LOG_DEBUG, "User-Agent: " + request.getHeader("User-Agent"), true);

	return true;
}

bool handleClientRequest(int client_sockfd, Request &request, Logger &logger)
{
	char buffer[1024];
	std::string fullRequest;
	request.setClientSocket(client_sockfd);

	while (true)
	{
		if (!readClientData(client_sockfd, buffer, fullRequest, logger))
			return false;
		if (request.isComplete(fullRequest))
		{
			if (!processRequest(request, fullRequest, logger))
				return false;
			if (!request.keepAlive())
				logger.logDebug(LOG_DEBUG, "Closing connection after response");
			return true;
		}
	}
}

void handleServerSocket(int server_fd, int epoll_fd, Logger &logger)
{
	handleNewConnection(server_fd, epoll_fd, logger);
	logger.logDebug(LOG_DEBUG, "New connection accepted", true);
}
void closeConnection(int client_fd, int epoll_fd)
{
	close(client_fd);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
}

void handleClientSocket(int client_fd, int epoll_fd, Request &request, Logger &logger)
{
	if (!handleClientRequest(client_fd, request, logger)) {
		closeConnection(client_fd, epoll_fd);
		return;
	}

	const ServerConfigs* serverConfig = getConfig().getServerConfig(getConfig().getServerSocket(client_fd));
	const LocationConfigs* locationConfig = getConfig().getLocationConfig(*serverConfig, request.getUri());
	std::string responseFull;
	Response response;

	logger.logDebug(LOG_DEBUG, "location.cgiConfig.cgiPath = " + 
		locationConfig->cgiPath, true);

	if (locationConfig && locationConfig->cgiEnabled) {
		logger.logDebug(LOG_DEBUG, "Entrou no CGI", true);
		
		CGI cgi(request, *serverConfig, *locationConfig);

		// ConfigUtils::printServerStruct(*serverConfig);


		response.setStatus(cgi.getReturnCode(), "OK");

		// response.setStatus(200, "OK");
		// response.setBodyWithContentType(cgiOutput, locationConfig->cgiConfig.cgiPath);
		responseFull = cgi.getReturnBody();
	}
	else { response.processRequest(request, serverConfig, logger); }

	if (responseFull.empty()) { 
		logger.logDebug(LOG_DEBUG, "Generating response", true);
		responseFull = response.generateResponse();
	}

	logger.logDebug(LOG_DEBUG, "Sending response to client", true);
	ssize_t bytes_sent = send(request.getClientSocket(), responseFull.c_str(), responseFull.size(), 0);

	if (bytes_sent == -1)
	{
		logger.logError(LOG_ERROR, "Error sending response");
		closeConnection(client_fd, epoll_fd);
	}
	else if (!request.getIsRequestValid() || !request.keepAlive())
	{
		logger.logDebug(LOG_DEBUG, "Closing connection after response");
		closeConnection(client_fd, epoll_fd);
	}
}



bool isServerSocket(int fd, const std::vector<int> &server_fds)
{
	return std::find(server_fds.begin(), server_fds.end(), fd) != server_fds.end();
}

void runServer(const std::vector<int> &server_fds, int epoll_fd, Logger &logger)
{
	epollEvent events[MAX_EVENTS];
	Request request;

	while (true)
	{
		int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_fds == -1)
		{
			logger.logError(LOG_ERROR, "epoll_wait failed");
			break;
		}

		for (int i = 0; i < num_fds; ++i)
		{
			int current_fd = events[i].data.fd;

			if (isServerSocket(current_fd, server_fds))
				handleServerSocket(current_fd, epoll_fd, logger);
			else if (events[i].events & EPOLLIN)
				handleClientSocket(current_fd, epoll_fd, request, logger);
		}
	}
}

void initializeServers(const std::vector<ServerConfigs> &servers, std::vector<int> &fds, EpollManager &epoll, Logger &logger)
{
	std::ostringstream log;
	for (size_t i = 0; i < servers.size(); ++i)
	{
		int sockfd = -1;
		int backlog = SOMAXCONN;
		if (!createServer(sockfd, servers[i].port, backlog, logger))
		{
			log << "Error creating server on port " << servers[i].port;
			logger.logError(LOG_ERROR, log.str(), true);
			continue;
		}
		fds.push_back(sockfd);
		getConfig().setSocketConfigMap(sockfd, &servers[i]);
		epoll.addToEpoll(sockfd);
	}
}

void cleanup(std::vector<int> &fds, Logger &logger)
{
	for (size_t i = 0; i < fds.size(); ++i)
	{
		close(fds[i]);
		logger.logDebug(LOG_DEBUG, "Socket closed", true);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return EXIT_FAILURE;
	}
	signal(SIGINT, signals);
	signal(SIGQUIT, signals);
	const std::string config_file_path = argv[1];
	std::vector<int> fds;
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	try
	{
		Config config(config_file_path, logger);
		setConfig(config);
		EpollManager epoll;
		initializeServers(config.getServers(), fds, epoll, logger);
		runServer(fds, epoll.getEpollFD(), logger);
		cleanup(fds, logger);
	}
	catch (const std::exception &e)
	{
		logger.logError(LOG_ERROR, e.what(), true);
		cleanup(fds, logger);
		std::string bye(e.what());
		if (bye == "bye bye")
			return EXIT_SUCCESS;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// void startEventLoop(Logger &logger)
// {
// 	epollEvent events[MAX_EVENTS];
// 	Request request;
// 	Response response;

// 	while (true)
// 	{
// 		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
// 		if (num_events == -1)
// 		{
// 			logger.logError(LOG_ERROR, "Error on epoll_wait");
// 			continue;
// 		}
// 		for (int i = 0; i < num_events; ++i)
// 		{
// 			int sockfd = events[i].data.fd;
// 			request = clientServerAccept(sockfd, logger);
// 			response.responseTratament(request, logger);
// 			std::string responseFull = response.generateResponse();
// 			send(request.getClientSocket(), responseFull.c_str(), responseFull.size(), 0);
// 			close(request.getClientSocket());
// 			// std::string response = requestHandler(request, logger);
// 			// std::string header = createHeaderRequest(request.uri, request.status, response.size());
// 			// response = header + response;
// 			// sendResponse(request.client_socket, response, logger);
// 		}
// 	}
// }

// Request clientServerAccept(int sockfd, Logger &logger)
// {
// 	sockaddrIn cli_addr;
// 	socklen_t clilen = sizeof(cli_addr);
// 	Request request;
// 	int newsockfd = accept(sockfd, (sockAddr *)&cli_addr, &clilen);
// 	addToEpoll(newsockfd);
// 	if (newsockfd < 0)
// 	{
// 		logger.logError(LOG_ERROR, "Error on accept");
// 		ft_error("Error on accept", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error on accept"));
// 	}
// 	std::ostringstream log, access_log;
// 	char buffer[4096];
// 	int n = recv(newsockfd, buffer, 4095, 0);
// 	if (n < 0)
// 	{
// 		logger.logError(LOG_ERROR, "Error reading from socket");
// 		close(newsockfd);
// 		ft_error("Error reading from socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error reading from socket"));
// 	}

// 	std::string str(buffer);
// 	if (!request.parseRequest(str))
// 	{
// 		request.setRequestIsValid(false);
// 		logger.logError(LOG_ERROR, "Error parsing request");
// 	}
// 	log << "Request received: " << request.getMethod() << " " << request.getUri() << " " << request.getHttpVersion();
// 	logger.logDebug(LOG_DEBUG, log.str());
// 	request.setClientSocket(newsockfd);
// 	access_log << inetNtop(cli_addr.sin_addr.s_addr) << " - -  \"" << request.getMethod() << " " << request.getUri() << " " << request.getHttpVersion() << "\" ";
// 	logger.logAccess(LOG_INFO, access_log.str());
// 	logger.logDebug(LOG_DEBUG, request.getMethod(), true);
// 	request.setRequestIsValid(true);
// 	return request;
// }