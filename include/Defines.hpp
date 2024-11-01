#ifndef DEFINES_HPP
#define DEFINES_HPP

/* Server Macros */
# define LOG_FILE "logs/debug.log"
# define LOG_ACCESS_FILE "logs/access.log"
# define LOG_ERROR_FILE "logs/error.log"
# define MAX_EVENTS 64
# define MIN_PORT_NUMBER 0
# define MAX_PORT_NUMBER 65535

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

/* Default Location Configs */
# define DEFAULT_EMPTY ""
# define DEFAULT_LOCATION_PATH "/"
# define DEFAULT_ROOT "./static"
# define DEFAULT_INDEX "index.html"
# define DEFAULT_UPLOAD_PATH "./static/uploads"

/* Default Server Configs*/
# define DEFAULT_PORT 8080
# define DEFAULT_HOST "127.0.0.1"
# define DEFAULT_SERVER_NAME "localhost"
# define DEFAULT_LIMIT_BODY_SIZE 1048576
# define DEFAULT_ERROR_400 "./static/errors/400.html"
# define DEFAULT_ERROR_403 "./static/errors/403.html"
# define DEFAULT_ERROR_404 "./static/errors/404.html"
# define DEFAULT_ERROR_405 "./static/errors/405.html"
# define DEFAULT_ERROR_500 "./static/errors/500.html"

/* CGI Macros */
# define PHP_EXECUTABLE "/usr/bin/php"
# define PYTHON_EXECUTABLE "/usr/bin/python3"
# define EXTENSION_PHP ".php"
# define EXTENSION_PY ".py"
# define TIMEOUT_ERROR 256

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
# define ERROR_EXTRA_VALUE "Extra Value In Configuration File!"

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
# define ERROR_INVALID_UPLOAD_ENABLED "Location: Invalid Upload Enabled Value!"
# define ERROR_INVALID_CGI_PATH "Location: Invalid CGI Path!"
# define ERROR_INVALID_CGI_EXTENSION "Location: Invalid CGI Extension!"

/* Request Error Macros */
# define ERROR_METHOD_NOT_ALLOWED "Method Not Allowed!"
# define ERROR_FORBIDDEN "Forbidden File or Directory!"
# define ERROR_NOT_FOUND "File Not Found!"
# define ERORR_CREATE_PIPE "Error Creating Pipe!"
# define ERROR_CREATE_FORK "Error Creating Fork!"
# define ERROR_CGI_EXECUTION "Error Executing CGI Script!"

#endif
