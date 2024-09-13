#ifndef DEFINES_HPP
#define DEFINES_HPP

/* Server Macros */
#define LOG_FILE "logs/debug.log"
#define LOG_ACCESS_FILE "logs/access.log"
#define LOG_ERROR_FILE "logs/error.log"
#define MAX_EVENTS 64

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
# define COLORIZE(color, text) \
	color << text << RESET

/* Error Macros */
# define ERROR_LOG_FILE "Error Opening Log File!"

#endif
