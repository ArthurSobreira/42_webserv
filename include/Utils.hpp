#ifndef UTILS_HPP
#define UTILS_HPP

#include "Includes.hpp"

class Logger;

typedef std::map<std::string, std::string> errorMap;
typedef struct sockaddr_in sockaddrIn;
typedef struct epoll_event epollEvent;
typedef struct sockaddr sockAddr;
typedef struct stat status;
typedef struct dirent diretory;

bool inetPton(const std::string &ip_str, uint32_t &out_binary_ip, Logger &logger);
void ft_error(const char *message, const char *function, const char *file, int line, const std::exception &e);
std::string inetNtop(uint32_t binary_ip);
std::string readFile(const std::string &path);
std::string getContentType(const std::string &uri);
std::string listDirectory(const std::string &dirPath);

bool createSocket(int &sockfd, int domain, int protocol, Logger &logger);
bool bindSocket(int &sockfd, const int &port, const uint32_t &ip, sockaddrIn &serv_addr, Logger &logger);
bool listenSocket(int &sockfd, int &backlog, Logger &logger);
void createServer(int &sockfd, const int &port, const uint32_t &ip, int &backlog, Logger &logger);

#endif
