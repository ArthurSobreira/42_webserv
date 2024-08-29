#ifndef DEFINES_HPP_
#define DEFINES_HPP_

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PINK "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

#define MAX_EVENTS 64

typedef struct sockaddr_in sockaddrIn;
typedef struct epoll_event epollEvent;
typedef struct sockaddr sockaddr;
typedef struct stat status;
typedef struct dirent diretory;

#endif // DEFINES_HPP_
