/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   main.cpp										   :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>	+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2024/09/03 11:55:07 by phenriq2		  #+#	#+#			 */
/*   Updated: 2024/09/10 12:16:24 by phenriq2		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

int epoll_fd;

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

Request clientServerAccept(int sockfd, Logger &logger){
	sockaddrIn cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	Request request;
	int newsockfd = accept(sockfd, (sockAddr*)&cli_addr, &clilen);
	if (newsockfd < 0)
	{
		logger.logError("ERROR", "Error on accept");
		ft_error("Error on accept", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error on accept"));
	}
	std::ostringstream log, access_log;
	char buffer[4096];
	int n = recv(newsockfd, buffer, 4095, 0);
	if (n < 0)
	{
		logger.logError("ERROR", "Error reading from socket");
		close(newsockfd);
		ft_error("Error reading from socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error reading from socket"));
	}

	std::string str(buffer);
	if(!request.parseRequest(str)){
		request.requestIsValid = false;
		logger.logError("ERROR", "Error parsing request");
	}
	log << "Request received: " << request.method << " " << request.uri << " " << request.http_version;
	logger.logDebug(log.str());
	request.client_socket = newsockfd;
	access_log << inetNtop(cli_addr.sin_addr.s_addr) << " - -  \"" << request.method << " " << request.uri << " " << request.http_version << "\" ";
	logger.logAccess(access_log.str());
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


void createServer(int &sockfd, const int &port, const uint32_t &ip, int &backlog, Logger &logger)
{
	sockaddrIn serv_addr;
	std::ostringstream log;
	if (!createSocket(sockfd, AF_INET, SOCK_STREAM, logger))
		ft_error("Error creating socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error creating socket"));
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		ft_error("Erro ao definir opção de socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	if (!bindSocket(sockfd, port, ip, serv_addr, logger))
	{
		close(sockfd);
		ft_error("Error binding socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error binding socket"));
	}
	if (!listenSocket(sockfd, backlog, logger))
	{
		close(sockfd);
		ft_error("Error listening on socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error listening on socket"));
	}
	log<<"Server created on "<<inetNtop(ip)<<":"<<port;
	logger.logDebug(log.str());
}


void populateServersAndPorts(std::string config_file_path, std::vector<uint32_t> &servers, std::vector<int> &ports, Logger &logger)
{
	(void) config_file_path;
	std::string server1 = "127.0.0.1";
	std::string server2 = "10.12.12.3";
	uint32_t ip1;
	uint32_t ip2;
	
	int porta1 = 13000;
	int porta2 = 13001;
	if (!inetPton(server1, ip1, logger))
		ft_error("Error converting IP to binary format", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error converting IP to binary format"));
	if (!inetPton(server2, ip2, logger))
		ft_error("Error converting IP to binary format", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error converting IP to binary format"));
	servers.push_back(ip1);
	servers.push_back(ip2);
	ports.push_back(porta1);
	ports.push_back(porta2);
}



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
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
	{
		throw std::runtime_error("Failed to add socket to epoll");
	}
}

void initializeServers(const std::vector<uint32_t>& servers, const std::vector<int>& ports, std::vector<int> &fds, Logger& logger)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		int sockfd = -1;
		int backlog = SOMAXCONN;
		const uint32_t ip = servers[i];
		createServer(sockfd, ports[i], ip, backlog, logger);
		fds.push_back(sockfd);
		addToEpoll(sockfd);
	}
}


void startEventLoop(Logger& logger)
{
	struct epoll_event events[MAX_EVENTS];
	Request request;
	Response response;

	while (true)
	{
		int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1)
		{
			logger.logError("ERROR", "Error on epoll_wait");
			continue;
		}

		for (int i = 0; i < num_events; ++i)
		{
			int sockfd = events[i].data.fd;
			request = clientServerAccept(sockfd, logger);

			// std::string response = requestHandler(request, logger);
			// std::string header = createHeaderRequest(request.uri, request.status, response.size());
			// response = header + response;
			// sendResponse(request.client_socket, response, logger);
		}
	}
}

void cleanup(std::vector<int>& fds)
{
	for (size_t i = 0; i < fds.size(); ++i)
		close(fds[i]);
	close(epoll_fd);
}

void signals(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM)
	{
		throw std::runtime_error("bye bye");
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
	std::vector<uint32_t> servers;
	std::vector<int> ports;
	std::vector<int> fds;
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);

	try
	{
		initializeEpoll();
		populateServersAndPorts(config_file_path, servers, ports, logger);
		initializeServers(servers, ports,fds, logger);
		startEventLoop(logger);
		cleanup(fds);
	}
	catch (const std::exception &e)
	{
		return EXIT_FAILURE;
	}

	std::cout << GREEN << "MAIN" << RESET << std::endl;
	return EXIT_SUCCESS;
}
