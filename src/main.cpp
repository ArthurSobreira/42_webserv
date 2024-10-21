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

void readClientData(int client_sockfd, char *buffer, std::stringstream &fullRequest, int &n, Logger &logger)
{
	n = recv(client_sockfd, buffer, 4096, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (n == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			logger.logDebug(LOG_DEBUG, "No data available to read, try again later.");
			n = 0;
		}
		else
			logger.logError(LOG_ERROR, "Error receiving data from client socket");
	}
	else if (n == 0)
		logger.logDebug(LOG_DEBUG, "Client disconnected");
	else
		fullRequest.write(buffer, n);
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
	std::stringstream fullRequest;
	int n = 0;
	do
	{
		readClientData(client_sockfd, buffer, fullRequest, n, logger);
	} while (n > 0);
	if (!request.parseRequest(fullRequest.str()))
	{
		logger.logError(LOG_ERROR, "Invalid HTTP request", true);
		return false;
	}
	std::string contentLength_str = request.getHeader("Content-Length");
	if (contentLength_str.empty())
	{
		std::cout << "Content-Length not found" << std::endl;
		return true;
	}
	size_t contentLength;
	std::stringstream contentLengthStream(contentLength_str);
	contentLengthStream >> contentLength;
	if (contentLength == 0)
	{
		std::cout << "Content-Length is 0" << std::endl;
		return true;
	}
	std::string body = fullRequest.str();
	int attempts = 0;
	const int maxAttempts = 10;  

	while (body.size() < contentLength && attempts < maxAttempts)
	{
		readClientData(client_sockfd, buffer, fullRequest, n, logger);
		body = fullRequest.str();
		// attempts++;
		if (n <= 0) 
		{
			logger.logError(LOG_ERROR, "Failed to read more data, closing connection", true);
			return false;
		}
	}

	if (attempts >= maxAttempts) 
	{
		logger.logError(LOG_ERROR, "Maximum attempts reached while reading request body", true);
		return false;
	}

	std::cout << "Request processed" << std::endl;
	if (request.isComplete(body)) 
		return true;
	std::cout << "Request not complete" << std::endl;
	return false;
}


void closeConnection(int client_fd, int epoll_fd)
{
	close(client_fd);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
}

void handleClientSocket(int client_fd, int epoll_fd, Request &request, Logger &logger)
{
	if (!handleClientRequest(client_fd, request, logger))
	{
		std::cout << "Error handling client request" << std::endl;
		closeConnection(client_fd, epoll_fd);
		return;
	}

	Config &config = getConfig();
	const ServerConfigs *serverConfig = config.getServerConfig(config.getServerSocket(client_fd));
	if (serverConfig == NULL)
	{
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

	if (locationConfig.cgiEnabled)
	{
		CGI cgi(request, *serverConfig, locationConfig);
		cgi.executeCGI();
		response.setStatus(cgi.getReturnCode(), cgi.getReasonPhrase());
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
