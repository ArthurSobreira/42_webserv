#ifndef DEFINES_HPP
#define DEFINES_HPP

/* Server Macros */
# define MAX_EVENTS 64
# define LOG_FILE "logs/debug.log"
# define LOG_ACCESS_FILE "logs/access.log"
# define LOG_ERROR_FILE "logs/error.log"

/* Colors Macros */
# define COLORIZE(color, text) \
	(std::string(color) + text + "\033[0m")

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

/* Logger Macros */
# define LOG_DEBUG "[DEBUG] "
# define LOG_INFO  "[INFO] "
# define LOG_WARN  "[WARNING] "
# define LOG_ERROR "[ERROR] "

/* Default CGI Configs */
# define DEFAULT_CGI_PATH "cgi-bin"
# define DEFAULT_CGI_EXT ".py"

/* Default Location Configs */
# define DEFAULT_LOCATION_PATH "/"
# define DEFAULT_ROOT "./static"
# define DEFAULT_INDEX "index.html"
# define DEFAULT_REDIRECT ""
# define DEFAULT_UPLOAD_PATH "./static/uploads"

/* Default Server Configs*/
# define DEFAULT_PORT 8080
# define DEFAULT_HOST "127.0.0.1"
# define DEFAULT_SERVER_NAME "localhost"
# define DEFAULT_LIMIT_BODY_SIZE 1048576
# define DEFAULT_ERROR_403 "./static/403.html"
# define DEFAULT_ERROR_404 "./static/404.html"

/* File Error Macros */
# define ERROR_OPEN_LOG_FILE "Could Not Open Log File!"
# define ERROR_OPEN_CONFIG_FILE "Could Not Opening Config File!"
# define ERROR_EMPTY_CONFIG_FILE "Config File Is Empty!"

/* Config File Formatting Error */
# define ERROR_INVALID_SERVER "Invalid Server Block!"
# define ERROR_INVALID_LOCATION "Invalid Location Block!"
# define ERROR_INVALID_LINE "Line Must End With a Semicolon ';' !"
# define ERROR_INVALID_KEY "Invalid Key in Configuration File!"
# define ERROR_MISSING_VALUE "Missing Value In Configuration File!"
# define ERROR_DUPLICATE_KEY "Duplicate Key In Configuration File!"

/* Server Extraction Error */
# define ERROR_INVALID_PORT "Server: Invalid Port Number!"
# define ERROR_INVALID_HOST "Server: Invalid Host Address!"
# define ERROR_INVALID_LIMIT_BODY_SIZE "Server: Invalid Limit Body Size!"
# define ERROR_INVALID_ERROR_PAGE "Server: Invalid Error Page!"

/* Location Extraction Error */
# define ERROR_INVALID_METHOD "Location: Invalid HTTP Method!"
# define ERROR_INVALID_LOCATION_PATH "Location: Invalid Location Path!"
# define ERROR_INVALID_ROOT "Location: Invalid Root Path!"
# define ERROR_INVALID_UPLOAD_PATH "Location: Invalid Upload Path!"
# define ERROR_INVALID_AUTOINDEX "Location: Invalid Autoindex Value!"

#endif
