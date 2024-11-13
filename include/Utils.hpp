#ifndef UTILS_HPP
#define UTILS_HPP

#include "Includes.hpp"
#include "Logger.hpp"

/* Global Variables */
extern volatile sig_atomic_t stop;
extern Logger logger;

typedef enum {
	INVALID = -1,
	GET = 1,
	POST,
	DELETE,
} httpMethod;

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
bool	inetPton(const std::string &ip_str);
std::string	inetNtop(uint32_t binary_ip);
std::string	readFile(const std::string &path);
std::string	getContentType(const std::string &uri);
std::string	removeLastSlashes(const std::string &uri);
std::string	getStringMethod( httpMethod method );
std::string	getErrorMessage(const std::string &status);
std::string	intToString(int value);
bool	counterOneSlash(const std::string &uri);
bool	createSocket(int &sockfd, int domain, int type);
bool	isDirectory(const std::string &path);
void	setupSignalHandlers(void);

#endif
