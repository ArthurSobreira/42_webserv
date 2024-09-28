#include "Server.hpp"
#include "Logger.hpp"
#include "Includes.hpp"

void testCreateSocket()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int sockfd;

	std::cout << "Test 1: Creating a valid socket..." << std::endl;
	if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger))
		std::cout << "Test 1 passed!" << std::endl;
	else
		std::cout << "Test 1 failed!" << std::endl;

	std::cout << "Test 2: Creating an invalid socket (invalid domain)..." << std::endl;
	if (!createSocket(sockfd, -1, SOCK_STREAM, logger))
		std::cout << "Test 2 passed!" << std::endl;
	else
		std::cout << "Test 2 failed!" << std::endl;

	closeSocket(sockfd, logger); // Fechar o socket no final do teste.
}

void testBindSocket()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int sockfd;
	sockaddr_in serv_addr;

	std::cout << "Test 6: Binding valid socket to port 8080..." << std::endl;
	if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger) &&
		bindSocket(sockfd, 8080, INADDR_ANY, serv_addr, logger)) // Corrigido aqui
		std::cout << "Test 6 passed!" << std::endl;
	else
		std::cout << "Test 6 failed!" << std::endl;

	closeSocket(sockfd, logger);
}

void testListenSocket()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int sockfd;
	int backlog = 5;
	sockaddr_in serv_addr;

	std::cout << "Test 7: Listening on valid socket..." << std::endl;
	if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger) &&
		configureSocket(sockfd, logger) &&
		bindSocket(sockfd, 8080, INADDR_ANY, serv_addr, logger) && // Corrigido aqui
		listenSocket(sockfd, backlog, logger)) // Corrigido aqui
		std::cout << "Test 7 passed!" << std::endl;
	else
		std::cout << "Test 7 failed!" << std::endl;

	closeSocket(sockfd, logger);
}

void testCreateServer()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int sockfd;
	int backlog = 5;

	std::cout << "Test 8: Creating a valid server..." << std::endl;
	if (createServer(sockfd, 8080, backlog, logger))
		std::cout << "Test 8 passed!" << std::endl;
	else
		std::cout << "Test 8 failed!" << std::endl;

	closeSocket(sockfd, logger);
}

void testLogErrorAndClose()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int sockfd;

	sockfd = open(LOG_FILE, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	std::cout << "Test 9: Logging an error and closing a socket..." << std::endl;
	if (logErrorAndClose("Error message", sockfd, logger))
		std::cout << "Test 9 passed!" << std::endl;
	else
		std::cout << "Test 9 failed!" << std::endl;
}

void testValidateBacklog()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int backlog = 5;

	std::cout << "Test 10: Validating a valid backlog..." << std::endl;
	if (validateBacklog(backlog, logger))
		std::cout << "Test 10 passed!" << std::endl;
	else
		std::cout << "Test 10 failed!" << std::endl;

	backlog = -1;
	std::cout << "Test 11: Validating an invalid backlog..." << std::endl;
	if (!validateBacklog(backlog, logger))
		std::cout << "Test 11 passed!" << std::endl;
	else
		std::cout << "Test 11 failed!" << std::endl;
}

void testConfigureSocket()
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	int sockfd;

	std::cout << "Test 12: Configuring a valid socket..." << std::endl;
	if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger) &&
		configureSocket(sockfd, logger))
		std::cout << "Test 12 passed!" << std::endl;
	else
		std::cout << "Test 12 failed!" << std::endl;

	closeSocket(sockfd, logger);
}

int main()
{
	// Teste individual de cada função
	testCreateSocket();
	testBindSocket();
	testListenSocket();
	testCreateServer();
	testLogErrorAndClose();
	testValidateBacklog();
	testConfigureSocket();

	return 0;
}
