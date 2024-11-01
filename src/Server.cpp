#include "Includes.hpp"
#include "Defines.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

void closeSocket(int &sockfd, Logger &logger)
{
	if (sockfd >= 0)
	{
		close(sockfd);
		logger.logDebug(LOG_DEBUG, "Socket closed");
		sockfd = -1;
	}
}

bool logErrorAndClose(const std::string &message, int &sockfd, Logger &logger)
{
	logger.logError(LOG_ERROR, message);
	closeSocket(sockfd, logger);
	return false;
}

bool validateBacklog(const int &backlog, Logger &logger)
{
	if (backlog < 0 || backlog > SOMAXCONN)
	{
		logger.logError(LOG_ERROR, "Invalid backlog number");
		return false;
	}
	return true;
}

bool createSocket(int &sockfd, int domain, int protocol, Logger &logger)
{
	sockfd = socket(domain, protocol, 0);
	if (sockfd < 0)
	{
		logger.logError(LOG_ERROR, "Error opening socket");
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Socket created");
	return true;
}

bool bindSocket(int &sockfd, const unsigned short &port, const uint32_t &ip, sockaddrIn &serv_addr, Logger &logger)
{
	logStream log;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ip;
	serv_addr.sin_port = htons(port);
	if (bind(sockfd, (sockAddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		log << "Error on binding to port " << port;
		return logErrorAndClose(log.str(), sockfd, logger);
	}
	log << "Binded to port " << port;
	logger.logDebug(LOG_DEBUG, log.str());
	return true;
}

bool listenSocket(int &sockfd, int &backlog, Logger &logger)
{
	logStream log;

	if (listen(sockfd, backlog) < 0)
		return logErrorAndClose("Error on listen", sockfd, logger);
	log << "Listening on socket with backlog: " << backlog;
	logger.logDebug(LOG_DEBUG, log.str());
	return true;
}

bool configureSocket(int sockfd, Logger &logger)
{
	int opt = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		logger.logError(LOG_ERROR, "Error on setsockopt");
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Socket configured");
	return true;
}

bool createServer(int &sockfd, const unsigned short &port, int &backlog, Logger &logger)
{
	sockaddrIn serv_addr;
	logStream log;
	uint32_t ip = INADDR_ANY;

	if (!validateBacklog(backlog, logger))
		return false;
	if (!createSocket(sockfd, AF_INET, SOCK_STREAM, logger))
		return false;
	if (!configureSocket(sockfd, logger))
		return false;
	if (!bindSocket(sockfd, port, ip, serv_addr, logger))
		return false;
	if (!listenSocket(sockfd, backlog, logger))
		return false;
	log << "Server created on " << inetNtop(ip) << ":" << port;
	logger.logDebug(LOG_DEBUG, log.str(), true);
	return true;
}
