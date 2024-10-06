#include "Request.hpp"
#include <sstream>

Request::Request() : method(""), uri(""), http_version(""), requestIsValid(false), allow_directory_listing(true) {}

bool Request::parseRequest(const std::string &raw_request)
{
	size_t body_start = raw_request.find("\r\n\r\n");
	std::cout << raw_request << std::endl;
	setRawRequest(raw_request);
	if (body_start == std::string::npos)
	{
		requestIsValid = false;
		return false;
	}

	std::string header_part = raw_request.substr(0, body_start);
	body = raw_request.substr(body_start + 4);

	std::istringstream stream(header_part);
	stream >> method >> uri >> http_version;

	if (!validateMethod() || !validateHttpVersion())
	{
		requestIsValid = false;
		return false;
	}

	std::string line;
	std::getline(stream, line);
	while (std::getline(stream, line) && line != "\r")
	{
		size_t pos = line.find(": ");
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			value.erase(value.find_last_not_of(" \n\r\t") + 1);
			headers[key] = value;
		}
	}

	if (headers.find("Content-Length") != headers.end())
	{
		std::istringstream content_length_stream(headers["Content-Length"]);
		size_t content_length;
		content_length_stream >> content_length;
		if (body.size() != content_length)
		{
			requestIsValid = false;
			return false;
		}
	}

	requestIsValid = true;
	return true;
}

bool Request::validateMethod()
{
	std::set<std::string> valid_methods;
	valid_methods.insert("GET");
	valid_methods.insert("POST");
	valid_methods.insert("DELETE");
	return valid_methods.find(method) != valid_methods.end();
}

bool Request::validateHttpVersion()
{
	return (http_version == "HTTP/1.1" || http_version == "HTTP/1.0");
}

bool Request::isComplete(const std::string &raw_request) const
{
	return raw_request.find("\r\n\r\n") != std::string::npos;
}

std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end())
		return it->second;
	return "";
}

bool Request::keepAlive() const
{
	std::map<std::string, std::string>::const_iterator it = headers.find("Connection");
	if (it != headers.end())
		return (it->second == "keep-alive");
	return (http_version == "HTTP/1.1");
}