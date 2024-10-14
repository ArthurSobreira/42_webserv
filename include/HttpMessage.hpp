#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <map>
#include <string>

class HttpMessage
{
	protected:
		std::map<std::string, std::string> headers;
		std::string http_version;
		std::string body;

	public:
		HttpMessage() : http_version("HTTP/1.1") {}

		// Getters
		std::string getHttpVersion() const { return http_version; }
		std::string getHeader(const std::string& key) const {
			std::map<std::string, std::string>::const_iterator it = headers.find(key);
			if (it != headers.end())
				return it->second;
			return "";
		}
		std::string getBody() const { return body; }

		// Setters
		void setHttpVersion(const std::string& version) { http_version = version; }
		void setHeader(const std::string& key, const std::string& value) { headers[key] = value; }
		void setBody(const std::string& b) { body = b; }

		// Utility methods
		bool isHeaderPresent(const std::string& key) const {
			return headers.find(key) != headers.end();
		}
};

#endif // HTTPMESSAGE_HPP
