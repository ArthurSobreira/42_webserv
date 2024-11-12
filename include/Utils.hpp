#ifndef UTILS_HPP
#define UTILS_HPP

#include "Includes.hpp"
#include "Logger.hpp"

class Config;

/* Global Variables */
extern volatile sig_atomic_t stop;
extern Logger logger;

/* Typedefs */
typedef std::map<std::string, std::string> stringMap;
typedef std::vector<std::string> stringVector;
typedef struct sockaddr_in sockaddrIn;
typedef struct epoll_event epollEvent;
typedef struct sockaddr sockAddr;
typedef struct stat status;
typedef struct dirent diretory;
typedef std::ostringstream logStream;

/* Utils Functions */
bool inetPton(const std::string &ip_str);
void ft_error(const char *message, const char *function, 
	const char *file, int line, const std::exception &e);
std::string inetNtop(uint32_t binary_ip);
std::string readFile(const std::string &path);
std::string getContentType(const std::string &uri);
std::string listDirectory(const std::string &dirPath);
std::string	removeLastSlashes(const std::string &uri);
void setConfig(Config &c);
Config &getConfig();
bool isDirectory(const std::string &path);
void signals(int sig);
void setupSignalForkHandlers();
void setupSignalHandlers();
bool createSocket(int &sockfd, int domain, int type);

#endif
