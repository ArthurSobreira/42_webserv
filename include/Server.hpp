#ifndef SERVER_HPP
#define SERVER_HPP
#include "Includes.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "EpollManager.hpp"
#include "Request.hpp"
#include <vector>
#include <sstream>
class Logger;
void closeSocket(int &sockfd, Logger &logger);
bool logErrorAndClose(const std::string &message, int &sockfd, Logger &logger);
bool validateBacklog(const int &backlog, Logger &logger);
bool createSocket(int &sockfd, int domain, int protocol, Logger &logger);
bool bindSocket(int &sockfd, const unsigned short &port, const uint32_t &ip, sockaddrIn &serv_addr, Logger &logger);
bool listenSocket(int &sockfd, int &backlog, Logger &logger);
bool createServer(int &sockfd, const unsigned short &port, int &backlog, Logger &logger);
bool configureSocket(int sockfd, Logger &logger);
void signals(int sig);





class Server
{
public:
    Server(const Config &config, Logger &logger);
    void start();

private:
    const Config &config;
    Logger &logger;
    std::vector<int> fdsVector;
    EpollManager epoll;

    void initializeServers();
    void runServer();
    void handleNewConnection(int server_sockfd);
    void handleClientSocket(int client_fd);
    void cleanup();
};


#endif // SERVER_HPP