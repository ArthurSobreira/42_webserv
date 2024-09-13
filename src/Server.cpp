#include "Includes.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

bool createSocket(int &sockfd, int domain, int protocol, Logger &logger)
{
	sockfd = socket(domain, protocol, 0);
	if (sockfd < 0)
	{
		logger.logError("ERROR", "Error opening socket");
		return false;
	}
	logger.logDebug("Socket created");
	return true;
}

bool bindSocket(int &sockfd, const int &port, const uint32_t &ip , sockaddrIn &serv_addr, Logger &logger)
{
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ip;
	if (port < 0 || port > 65535)
	{
		logger.logError("ERROR", "Invalid port number");
		return false;
	}
	serv_addr.sin_port = htons(port);
	std::ostringstream log;
	if (bind(sockfd, (sockAddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		log << "Error on binding to port " << port;
		logger.logError("ERROR", log.str());
		return false;
	}
	log << "Binded to port " << port;
	logger.logDebug(log.str());
	return true;
}

bool listenSocket(int &sockfd, int &backlog, Logger &logger)
{
	std::ostringstream log;
	if (backlog < 0 || backlog > SOMAXCONN)
	{
		log << "Invalid backlog number: " << backlog;
		logger.logError("ERROR", log.str());
		return false;
	}
	if (listen(sockfd, backlog) < 0)
	{
		logger.logError("ERROR", "Error on listening");
		return false;
	}
	log << "Listening on socket with backlog: " << backlog;
	logger.logDebug(log.str());
	return true;
}
