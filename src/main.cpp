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
	epollEvent ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev) == -1)
	{
		logger.logError(LOG_ERROR, "Failed to add client socket to epoll");
		close(client_sockfd);
		return;
	}
	else
		logger.logDebug(LOG_DEBUG, "New client connection accepted");
	getConfig().setSocketServerMap(client_sockfd, server_sockfd);
}

bool readClientData(int client_sockfd, char *buffer, std::string &fullRequest, Logger &logger)
{
	int n = recv(client_sockfd, buffer, 4096, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (n == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			logger.logDebug(LOG_DEBUG, "No data available to read, try again later.");
			return true;
		}
		else
		{
			logger.logError(LOG_ERROR, "Error receiving data from client socket");
			return false;
		}
	}
	else if (n == 0)
	{
		logger.logDebug(LOG_DEBUG, "Client disconnected");
		return false;
	}
	else
	{
		fullRequest.append(buffer, n);
		std::cout << std::endl;
		std::cout << fullRequest << std::endl;
		std::cout << std::endl;
		return true;
	}
}

bool processRequest(Request &request, const std::string &fullRequest, Logger &logger)
{
	if (!request.parseRequest(fullRequest))
	{
		logger.logError(LOG_ERROR, "Invalid HTTP request", true);
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Request processed", true);
	logger.logDebug(LOG_DEBUG, "Method: " + request.getMethod() + ", URI: " + request.getUri() + ", HTTP Version: " + request.getHttpVersion(), true);
	logger.logDebug(LOG_DEBUG, "User-Agent: " + request.getHeader("User-Agent"), true);

	return true;
}

bool handleClientRequest(int client_sockfd, Request &request, Logger &logger)
{
	char buffer[4096];
	std::string fullRequest;

	while (true)
	{
		if (!readClientData(client_sockfd, buffer, fullRequest, logger))
			return false;
		if (request.isComplete(fullRequest))
		{
			if (!processRequest(request, fullRequest, logger))
				return false;
			// if (!request.keepAlive())
			// 	logger.logDebug(LOG_DEBUG, "Closing connection after response");

			return true;
		}
	}
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

	Config &config = getConfig();
	const ServerConfigs* serverConfig = config.getServerConfig(config.getServerSocket(client_fd));
	if (serverConfig == NULL) {
		logger.logError(LOG_ERROR, "Server config not found");
		closeConnection(client_fd, epoll_fd);
		return;
	}

	bool locationFound = false;
	const LocationConfigs locationConfig = config.getLocationConfig(*serverConfig, 
		request.getUri(), locationFound);
	std::string responseFull;
	Response response;

	// if (!locationFound) {
	// 	logger.logDebug(LOG_DEBUG, "Location not found :(", true);
	// 	response.handleError(404, serverConfig->errorPages.at("404"), "File not found", logger);
	// }

	if (locationConfig.cgiEnabled) {
		CGI cgi(request, *serverConfig, locationConfig);

		cgi.executeCGI();
		response.setStatus(cgi.getReturnCode(), "OK");
		response.setBody(cgi.getReturnBody());
		responseFull = response.generateResponse();
	} else {
		response.processRequest(request, serverConfig, logger); 
		responseFull = response.generateResponse();
	}

	ssize_t bytes_sent = send(client_fd, responseFull.c_str(), responseFull.size(), 0);

	if (bytes_sent == -1)
	{
		logger.logError(LOG_ERROR, "Invalid Content-Length header");
		closeConnection(client_fd, epoll_fd);
		return;
	}

	// std::ofstream outfile("request_body.pdf", std::ios::binary);
	// if (outfile.is_open())
	// {
	// 	outfile.write(request.getBody().data(), request.getBody().size()); // write em vez de <<
	// 	outfile.close();
	// 	logger.logDebug(LOG_DEBUG, "Request body written to file", true);
	// }
	// else
	// {
	// 	logger.logError(LOG_ERROR, "Failed to open file to write request body");
	// }

	// // Response response;
	// response.processRequest(request,getConfig().getServerConfig(getConfig().getServerSocket(client_fd)), logger);
	// std::string responseFull = response.generateResponse();

	logger.logDebug(LOG_DEBUG, "Sending response to client", true);
	// ssize_t bytes_sent = send(client_fd, responseFull.c_str(), responseFull.size(), 0);

	// if (bytes_sent == -1)
	// {
	// 	logger.logError(LOG_ERROR, "Error sending response");
	// 	closeConnection(client_fd, epoll_fd);
	// }
	// else if (!request.isRequestValid() || !request.keepAlive())
	// {
	// 	logger.logDebug(LOG_DEBUG, "Closing connection after response");
	closeConnection(client_fd, epoll_fd);
	// }
}

bool isServerSocket(int fd, const std::vector<int> &server_fds)
{
	return std::find(server_fds.begin(), server_fds.end(), fd) != server_fds.end();
}

void runServer(const std::vector<int> &server_fds, int epoll_fd, Logger &logger)
{
	epollEvent events[MAX_EVENTS];

	while (true)
	{
		int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_fds == -1)
		{
			if (errno == EINTR)
				continue;
			logger.logError(LOG_ERROR, "epoll_wait failed");
			break;
		}
		for (int i = 0; i < num_fds; ++i)
		{
			int current_fd = events[i].data.fd;
			if (isServerSocket(current_fd, server_fds))
				handleNewConnection(current_fd, epoll_fd, logger);
			else if (events[i].events & EPOLLIN)
			{
				Request request;
				handleClientSocket(current_fd, epoll_fd, request, logger);
			}
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				logger.logDebug(LOG_DEBUG, "Client disconnected or socket error");
				close(current_fd);
			}
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
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	std::vector<int> fdsVector;
	try
	{
		Config config(argv[1], logger);
		const std::vector<ServerConfigs> &servers = config.getServers();
		EpollManager epoll;
		setConfig(config);

		initializeServers(servers, fdsVector, epoll, logger);
		runServer(fdsVector, epoll.getEpollFD(), logger);
		cleanup(fdsVector, logger);
	}
	catch (const std::exception &e)
	{
		logger.logError(LOG_ERROR, e.what(), true);
		cleanup(fdsVector, logger);
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