
#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "Includes.hpp"

// Class declaration
class Request
{
private:
	std::string method;
	std::string uri;
	std::string http_version;
	bool requestIsValid;
	int client_socket;
	bool allow_directory_listing;
	std::map<std::string, std::string> headers;
	std::string body;

public:
	// Método para parsear a requisição do cliente
	bool parseRequest(const std::string &raw_request);
	Request();
	// Getters
	std::string getMethod() const { return method; }
	std::string getUri() const { return uri; }
	std::string getHttpVersion() const { return http_version; }
	bool isRequestValid() const { return requestIsValid; }
	int getClientSocket() const { return client_socket; }
	bool isAllowDirectoryListing() const { return allow_directory_listing; }
	std::map<std::string, std::string> getHeaders() const { return headers; }
	std::string getBody() const { return body; }

	// Setters
	void setMethod(const std::string &m) { method = m; }
	void setUri(const std::string &u) { uri = u; }
	void setHttpVersion(const std::string &hv) { http_version = hv; }
	void setRequestIsValid(bool valid) { requestIsValid = valid; }
	void setClientSocket(int socket) { client_socket = socket; }
	void setAllowDirectoryListing(bool allow) { allow_directory_listing = allow; }
	void setHeaders(const std::map<std::string, std::string> &h) { headers = h; }
	void setBody(const std::string &b) { body = b; }
};


#endif // REQUEST_HPP_
