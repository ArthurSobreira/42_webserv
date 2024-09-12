/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 17:56:54 by phenriq2          #+#    #+#             */
/*   Updated: 2024/09/05 19:17:03 by phenriq2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"

bool TestCreateSocket(Logger &logger)
{

	int sockfd = -1;
	createSocket(sockfd, AF_INET, SOCK_STREAM, logger);
	if (sockfd == -1)
		return false;
	close(sockfd);
	return true;
}

bool TestBindSocket(Logger &logger)
{
	int sockfd = -1;
	int port = 8080;
	uint32_t ip;
	createSocket(sockfd, AF_INET, SOCK_STREAM, logger);
	inetPton("127.0.0.1", ip, logger);
	sockaddrIn serv_addr;
	if (!bindSocket(sockfd, port, ip, serv_addr, logger))
		return false;
	close(sockfd);
	return true;
}

bool TestListenSocket(Logger &logger)
{
	int sockfd = -1;
	int port = 8080;
	int backlog = SOMAXCONN;
	uint32_t ip;
	createSocket(sockfd, AF_INET, SOCK_STREAM, logger);
	sockaddrIn serv_addr;
	inetPton("127.0.0.1", ip, logger);
	if (!bindSocket(sockfd, port, ip, serv_addr, logger))
		return false;
	if (!listenSocket(sockfd, backlog, logger))
		return false;
	close(sockfd);
	return true;
}



bool TestLogClass(Logger &logger)
{
	logger.logDebug("Testando logDebug");
	logger.logError("ERROR", "Testando logError");
	logger.logAccess("127.0.0.1", "GET /index.html HTTP/1.1", 200, 1024);
	std::string content = readFile(LOG_FILE);
	std::string contentAccess = readFile(LOG_ACCESS_FILE);
	std::string contentError = readFile(LOG_ERROR_FILE);
	if (content.find("Testando logDebug") == std::string::npos)
		return false;
	if (contentError.find("Testando logError") == std::string::npos)
		return false;
	if (contentAccess.find("127.0.0.1") == std::string::npos)
		return false;
	return true;
}

bool TestInetPton(Logger &logger)
{
	uint32_t ip;
	if (!inetPton("127.0.0.1", ip, logger))
		return false;
	if (ip != 16777343)
		return false;
	return true;
}

bool TestInetNtop()
{
	uint32_t ip = 16777343;
	std::string ip_str = inetNtop(ip);
	if (ip_str != "127.0.0.1")
		return false;
	return true;
}

int main(int argc, char **argv)
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return 1;
	}
	setLogger(logger);
	if (!TestLogClass(logger))
	{
		std::cerr << COLORIZE(RED, "TestLogClass falhou") << std::endl;
		return 1;
	}
	std::cout << COLORIZE(GREEN, "TestLogClass passou") << std::endl;
	if (!TestInetPton(logger))
	{
		std::cerr << COLORIZE(RED, "TestInetPton falhou") << std::endl;
		return 1;
	}
	std::cout << COLORIZE(GREEN, "TestInetPton passou") << std::endl;
	if (!TestInetNtop())
	{
		std::cerr << COLORIZE(RED, "TestInetNtop falhou") << std::endl;
		return 1;
	}
	std::cout << COLORIZE(GREEN, "TestInetNtop passou") << std::endl;
	if (!TestCreateSocket(logger))
	{
		std::cerr << COLORIZE(RED, "TestCreateSocket falhou") << std::endl;
		return 1;
	}
	std::cout << COLORIZE(GREEN, "TestCreateSocket passou") << std::endl;
	if (!TestBindSocket(logger))
	{
		std::cerr << COLORIZE(RED, "TestBindSocket falhou") << std::endl;
		return 1;
	}
	std::cout << COLORIZE(GREEN, "TestBindSocket passou") << std::endl;
	if (!TestListenSocket(logger))
	{
		std::cerr << COLORIZE(RED, "TestListenSocket falhou") << std::endl;
		return 1;
	}
	std::cout << COLORIZE(GREEN, "TestListenSocket passou") << std::endl;

	return 0;
}