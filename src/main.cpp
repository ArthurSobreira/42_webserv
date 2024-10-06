#include "Includes.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Server.hpp"
#include "EpollManager.hpp"

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
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &ev) == -1)
	{
		logger.logError(LOG_ERROR, "Failed to add client socket to epoll");
		close(client_sockfd);
	}
	else
		logger.logDebug(LOG_DEBUG, "New client connection accepted");
}


bool handleClientRequest(int client_sockfd, int epoll_fd, Request &request, Logger &logger)
{
	char buffer[1024];
	std::string fullRequest;
	request.setClientSocket(client_sockfd);

	while (true)
	{
		int n = read(client_sockfd, buffer, sizeof(buffer));

		if (n == -1)
		{
			logger.logError(LOG_ERROR, "Error reading from client socket");
			close(client_sockfd);
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sockfd, NULL);
			return false;
		}
		else if (n == 0)
		{
			logger.logDebug(LOG_DEBUG, "Client disconnected");
			close(client_sockfd);
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sockfd, NULL);
			return false;
		}
		else
		{
			fullRequest.append(buffer, n);

			if (request.isComplete(fullRequest))
			{
				if (!request.parseRequest(fullRequest))
				{
					logger.logError(LOG_ERROR, "Invalid HTTP request");
					close(client_sockfd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sockfd, NULL);
					return false;
				}

				logger.logDebug(LOG_DEBUG, "Request processed");

				std::string method = request.getMethod();
				std::string uri = request.getUri();
				std::string user_agent = request.getHeader("User-Agent");

				logger.logDebug(LOG_DEBUG, "Method: " + method + ", URI: " + uri);
				logger.logDebug(LOG_DEBUG, "User-Agent: " + user_agent);

				if (!request.keepAlive())
				{
					logger.logDebug(LOG_DEBUG, "Closing connection after response");
					close(client_sockfd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sockfd, NULL);
				}
				return true;
			}
		}
	}
}

void runServer(const std::vector<int> &server_fds, int epoll_fd, Logger &logger)
{
	struct epoll_event events[MAX_EVENTS];
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
			bool isServerSocket = false;

			for (size_t j = 0; j < server_fds.size(); ++j)
			{
				if (events[i].data.fd == server_fds[j])
				{
					handleNewConnection(server_fds[j], epoll_fd, logger);
					logger.logDebug(LOG_DEBUG, "New connection accepted", true);
					isServerSocket = true;
					break;
				}
			}

			if (!isServerSocket && (events[i].events & EPOLLIN))
			{
				handleClientRequest(events[i].data.fd, epoll_fd, request, logger);
				Response response;
				response.processRequest(request, logger);
				std::string responseFull = response.generateResponse();
				send(request.getClientSocket(), responseFull.c_str(), responseFull.size(), 0);
				if (!request.keepAlive())
				{
					close(request.getClientSocket());
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, request.getClientSocket(), NULL);
				}
				break;
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