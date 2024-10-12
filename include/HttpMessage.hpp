#ifndef HTTPMESSAGE_HPP_
#define HTTPMESSAGE_HPP_

#include <map>
#include <string>

class HttpMessage {
protected:
	std::string method;
	std::string uri;
	std::string http_version;
	std::map<std::string, std::string> headers;
	std::string body;
	bool requestIsValid;

public:
	HttpMessage() : method(""), uri(""), body("") {}

	// Getters
	std::string getMethod() const { return method; }
	std::string getUri() const { return uri; }
	std::string getBody() const { return body; }
	std::string getHeader(const std::string &key) const {
		std::map<std::string, std::string>::const_iterator it = headers.find(key);
		if (it != headers.end())
			return it->second;
		return "";
	}
	std::string getHttpVersion() const { return http_version; }
	bool getIsRequestValid() const { return requestIsValid; }

	// Setters
	void setMethod(const std::string &m) { method = m; }
	void setUri(const std::string &u) { uri = u; }
	void setBody(const std::string &b) { body = b; }
	void setHeader(const std::string &key, const std::string &value) { headers[key] = value; }
	void setHttpVersion(const std::string &hv) { http_version = hv; }
	void setRequestIsValid(bool valid) { requestIsValid = valid; }
};

#endif // HTTPMESSAGE_HPP_
