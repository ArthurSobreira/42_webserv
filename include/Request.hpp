
#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "Includes.hpp"

// Class declaration
class Request {
public:
	std::string method;
	std::string uri;
	std::string http_version;
	bool requestIsValid;
	int client_socket;
	bool allow_directory_listing;
	std::map<std::string, std::string> headers;
	std::string body;
		
	// Método para parsear a requisição do cliente
	bool parseRequest(const std::string &raw_request);
	Request();
};

#endif // REQUEST_HPP_
