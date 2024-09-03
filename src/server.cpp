// #include "Server.hpp"

// Server::Server(const std::string &config_path)
// {
// 	loadConfiguration(config_path);
// 	setupSocket();
// 	setupRoutes();
// 	return;
// }

// // Copy constructor
// Server::Server(const Server &other)
// {
// 	*this = other;
// 	return;
// }

// // Copy assignment overload
// Server &Server::operator=(const Server &rhs)
// {
// 	(void)rhs;
// 	return *this;
// }

// // Default destructor
// Server::~Server() { return; }

// void Server::loadConfiguration(const std::string &config_path)
// {}
// void Server::setupSocket() {}
// void Server::setupRoutes() {}
// void Server::handleRequest(int client_fd) {}
// void Server::sendResponse(int client_fd, const std::string &response) {}
// std::string Server::generateErrorResponse(int error_code) {}
// void Server::forkAndHandle(int client_fd) {}
