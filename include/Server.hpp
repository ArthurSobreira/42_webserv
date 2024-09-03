// #ifndef SERVER_HPP
// #define SERVER_HPP

// #include <string>
// #include <map>
// #include <sys/socket.h>
// #include <netinet/in.h>


// class Server
// {
// private:
// 	int port;
// 	std::string _server_name;
// 	std::string _root_directory;
// 	std::string _error_log_path;
// 	std::string _access_log_path;
// 	int _server_fd;
// 	struct sockaddr_in _server_addr;

// 	std::map<int, std::string> error_pages;

// 	void loadConfiguration(const std::string &config_path);
// 	void setupSocket();
// 	void setupRoutes();
// 	void handleRequest(int client_fd);
// 	void sendResponse(int client_fd, const std::string &response);
// 	std::string generateErrorResponse(int error_code);
// 	void forkAndHandle(int client_fd);

// public:
// 	Server(const std::string &config_path);
// 	~Server();

// 	void start();
// 	void stop();
// 	void handleSignal(int signal);
// };

// #endif // SERVER_HPP