#ifndef DEFINES_HPP_
#define DEFINES_HPP_

#define MAX_EVENTS 64

typedef struct sockaddr_in sockaddrIn;
typedef struct epoll_event epollEvent;
typedef struct sockaddr sockaddr;
typedef struct stat status;
typedef struct dirent diretory;

#define LOG_FILE "logs/server.log"

/* Colors Macros */
# define RESET "\033[0m"
# define BLACK "\033[30m"
# define GRAY "\033[90m"
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define WHITE "\033[37m"

# define COLORIZE(color, text) color << text << RESET

#endif // DEFINES_HPP_
