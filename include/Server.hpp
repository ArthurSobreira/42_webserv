#ifndef SERVER_HPP
#define SERVER_HPP
#include "Includes.hpp"
#include "Utils.hpp"
class Logger;
void closeSocket(int &sockfd, Logger &logger);
bool logErrorAndClose(const std::string &message, int &sockfd, Logger &logger);
bool validateBacklog(const int &backlog, Logger &logger);
bool createSocket(int &sockfd, int domain, int protocol, Logger &logger);
bool bindSocket(int &sockfd, const unsigned short &port, const uint32_t &ip, sockaddr_in &serv_addr, Logger &logger); // Corrigido aqui
bool listenSocket(int &sockfd, int &backlog, Logger &logger); // Corrigido aqui
bool createServer(int &sockfd, const unsigned short &port, int &backlog, Logger &logger);
bool configureSocket(int sockfd, Logger &logger);
#endif // SERVER_HPP