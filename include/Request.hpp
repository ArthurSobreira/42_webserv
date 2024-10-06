#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "Includes.hpp"
#include <map>
#include <string>
#include <sstream>
#include <set>

// Classe Request para parsing de requisições HTTP
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
	std::string raw_request;

public:
	Request();
	bool parseRequest(const std::string &raw_request);
	bool isComplete(const std::string &raw_request) const;
	// Getters
	std::string getMethod() const { return method; }
	std::string getUri() const { return uri; }
	std::string getHttpVersion() const { return http_version; }
	bool getIsRequestValid() const { return requestIsValid; }
	int getClientSocket() const { return client_socket; }
	bool getIsAllowDirectoryListing() const { return allow_directory_listing; }
	std::map<std::string, std::string> getHeaders() const { return headers; }
	std::string getBody() const { return body; }
	std::string getHeader(const std::string &key) const;
	bool keepAlive() const;
	std::string getRawRequest() const { return raw_request; }

	// Setters
	void setMethod(const std::string &m) { method = m; }
	void setUri(const std::string &u) { uri = u; }
	void setHttpVersion(const std::string &hv) { http_version = hv; }
	void setRequestIsValid(bool valid) { requestIsValid = valid; }
	void setClientSocket(int socket) { client_socket = socket; }
	void setAllowDirectoryListing(bool allow) { allow_directory_listing = allow; }
	void setHeaders(const std::map<std::string, std::string> &h) { headers = h; }
	void setBody(const std::string &b) { body = b; }
	void setRawRequest(const std::string &raw) { raw_request = raw; }
	bool validateMethod();

private:
	bool validateHttpVersion();
};

#endif // REQUEST_HPP_
