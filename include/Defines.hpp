#ifndef DEFINES_HPP
#define DEFINES_HPP

/* Server Macros */
# define LOG_FILE "logs/debug.log"
# define LOG_ACCESS_FILE "logs/access.log"
# define LOG_ERROR_FILE "logs/error.log"
# define MAX_EVENTS 64

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
# define ERROR_OPEN_LOG_FILE "Could Not Open Log File!"
# define ERROR_OPEN_CONFIG_FILE "Could Not Opening Config File!"
# define ERROR_NOT_CONFIG_FILE "Config File Not Passed!"
# define ERROR_EMPTY_CONFIG_FILE "Config File Is Empty!"
# define ERROR_INVALID_SERVER "Invalid Server Block!"
# define ERROR_EXTRA_CLOSE_BRACE "Extra closing brace '}' detected!"
# define ERROR_UNCLOSED_BLOCK "Unclosed server block detected!"

#endif
