#include "Includes.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include "Server.hpp"

int epoll_fd;
void addToEpoll(int sockfd);
// std::string getHttpError(int status_code)
// {
// 	switch (status_code)
// 	{
// 	case 200:
// 		return "OK";
// 	case 400:
// 		return "Bad Request";
// 	case 404:
// 		return "Not Found";
// 	case 500:
// 		return "Internal Server Error";
// 	default:
// 		return "Unknown";
// 	}
// }

// std::string getContentType(const std::string &path)
// {
// 	if (path.find(".html") != std::string::npos)
// 		return "text/html";
// 	if (path.find(".css") != std::string::npos)
// 		return "text/css";
// 	if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
// 		return "image/jpeg";
// 	if (path.find(".png") != std::string::npos)
// 		return "image/png";
// 	if (path.find(".gif") != std::string::npos)
// 		return "image/gif";
// 	if (path.find(".ico") != std::string::npos)
// 		return "image/x-icon";
// 	if (path.find(".js") != std::string::npos)
// 		return "application/javascript";
// 	if (path.find(".json") != std::string::npos)
// 		return "application/json";
// 	return "text/plain";
// }

// std::string createHeaderRequest(const std::string &path, int status_code, int content_length)
// {
// 	std::ostringstream header;
// 	header << "HTTP/1.1 " << status_code << " " << getHttpError(status_code) << "\r\n";
// 	header << "Content-Type: " << getContentType(path) << "\r\n";
// 	header << "Content-Length: " << content_length << "\r\n";
// 	header << "Connection: close\r\n\r\n";
// 	return header.str();
// }

Request clientServerAccept(int sockfd, Logger &logger)
{
	sockaddrIn cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	Request request;
	int newsockfd = accept(sockfd, (sockAddr *)&cli_addr, &clilen);
	addToEpoll(newsockfd);
	if (newsockfd < 0)
	{
		logger.logError(LOG_ERROR, "Error on accept");
		ft_error("Error on accept", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error on accept"));
	}
	std::ostringstream log, access_log;
	char buffer[4096];
	int n = recv(newsockfd, buffer, 4095, 0);
	if (n < 0)
	{
		logger.logError(LOG_ERROR, "Error reading from socket");
		close(newsockfd);
		ft_error("Error reading from socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error reading from socket"));
	}

	std::string str(buffer);
	if (!request.parseRequest(str))
	{
		request.setRequestIsValid(false);
		logger.logError(LOG_ERROR, "Error parsing request");
	}
	log << "Request received: " << request.getMethod() << " " << request.getUri() << " " << request.getHttpVersion();
	logger.logDebug(LOG_DEBUG, log.str());
	request.setClientSocket(newsockfd);
	access_log << inetNtop(cli_addr.sin_addr.s_addr) << " - -  \"" << request.getMethod() << " " << request.getUri() << " " << request.getHttpVersion() << "\" ";
	logger.logAccess(LOG_INFO, access_log.str());
	logger.logDebug(LOG_DEBUG, request.getMethod(), true);
	request.setRequestIsValid(true);
	return request;
}

// std::string requestHandler(t_request &request, Logger &logger)
// {
// 	std::ostringstream log;
// 	if(request.method.empty() || request.uri.empty() || request.http_version.empty())
// 	{
// 		logger.logError("ERROR", "Error parsing request");
// 		std::string response = readFile("static/405.html");
// 		request.status = 405;
// 		return response;
// 	}
// 	std::string path = "static" + request.uri;
// 	if (path[path.size() - 1] == '/')
// 		path += "index.html";
// 	std::string response;
// 	status status;
// 	if (stat(path.c_str(), &status) != 0)
// 	{
// 		logger.logError("ERROR", "File not found");
// 		response = readFile("static/404.html");
// 		request.status = 404;
// 		return response;
// 	}
// 	if(S_ISDIR(status.st_mode))
// 	{
// 		logger.logError("ERROR", "Path is a directory");
// 		response = readFile("static/404.html");
// 		request.status = 404;
// 		return response;
// 	}
// 	if(access(path.c_str(), R_OK) != 0)
// 	{
// 		logger.logError("ERROR", "File not readable");
// 		response = readFile("static/403.html");
// 		request.status = 403;
// 		return response;
// 	}
// 	response = readFile(path);
// 	return response;
// }

// void sendResponse(int sockfd, const std::string &response, Logger &logger)
// {
// 	int n = send(sockfd, response.c_str(), response.size(), 0);
// 	if (n < 0)
// 	{
// 		logger.logError("ERROR", "Error writing to socket");
// 	}
// 	close(sockfd);
// }

void initializeEpoll()
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		throw std::runtime_error("Failed to create epoll instance");
	}
}
void addToEpoll(int sockfd)
{
	epollEvent event;
	event.events = EPOLLIN;
	event.data.fd = sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
	{
		throw std::runtime_error("Failed to add socket to epoll");
	}
}

void initializeServers(const std::vector<ServerConfigs> &servers, std::vector<int> &fds, Logger &logger)
{
	std::ostringstream log;
	for (size_t i = 0; i < servers.size(); ++i)
	{
		int sockfd = -1;
		int backlog = SOMAXCONN;
		if(!createServer(sockfd, servers[i].port, backlog, logger))
		{
			log << "Error creating server on port " << servers[i].port;
			logger.logError(LOG_ERROR, log.str(), true);
			continue;
		}
		fds.push_back(sockfd);
		addToEpoll(sockfd);
	}
}

void startEventLoop(Logger &logger)
{
	epollEvent events[MAX_EVENTS];
	Request request;
	Response response;

	while (true)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1)
		{
			logger.logError(LOG_ERROR, "Error on epoll_wait");
			continue;
		}
		for (int i = 0; i < num_events; ++i)
		{
			int sockfd = events[i].data.fd;
			request = clientServerAccept(sockfd, logger);
			response.responseTratament(request, logger);
			std::string responseFull = response.generateResponse();
			send(request.getClientSocket(), responseFull.c_str(), responseFull.size(), 0);
			close(request.getClientSocket());
			// std::string response = requestHandler(request, logger);
			// std::string header = createHeaderRequest(request.uri, request.status, response.size());
			// response = header + response;
			// sendResponse(request.client_socket, response, logger);
		}
	}
}

void cleanup(std::vector<int> &fds)
{
	for (size_t i = 0; i < fds.size(); ++i)
		close(fds[i]);
	close(epoll_fd);
}

void signals(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM) {
		throw std::runtime_error("bye bye");
	}
}

int main(int argc, char **argv)
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	if (argc != 2) {
		logger.logError(LOG_ERROR, ERROR_NOT_CONFIG_FILE, true);
		return EXIT_FAILURE;
	}
	signal(SIGINT, signals);
	signal(SIGQUIT, signals);
	const std::string config_file_path = argv[1];
	std::vector<int> fds;
	try
	{
		Config config(config_file_path, logger);

		initializeEpoll();
		initializeServers(config.getServers(), fds, logger);
		startEventLoop(logger);
		cleanup(fds);
	}
	catch (const std::exception &e)
	{
		logger.logError(LOG_ERROR, e.what(), true);
		cleanup(fds);
		return EXIT_FAILURE;
	}

	std::cout << GREEN << "MAIN" << RESET << std::endl;
	return EXIT_SUCCESS;
}
