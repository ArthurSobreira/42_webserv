#include "Request.hpp"
#include "Includes.hpp"

Request::Request() : HttpMessage(), method(""), uri(""), requestIsValid(false) {}

bool Request::parseRequest(const std::string &raw_request)
{
	rawRequest = raw_request;
	std::cout << "raw_request: \n" << raw_request << std::endl;
	size_t body_start = raw_request.find("\r\n\r\n");
	if (body_start == std::string::npos)
	{
		std::cout << "debbug 01" << std::endl;
		return false;
	}

	std::string header_part = raw_request.substr(0, body_start);
	body = raw_request.substr(body_start + 4);

	if (!parseStartLine(header_part))
	{
		std::cout << "debbug 02" << std::endl;
		return false;
	}

	if (!parseHeaders(header_part))
	{
		std::cout << "debbug 03" << std::endl;
		return false;
	}

	requestIsValid = true;
	return true;
}

int hexstr_to_int(const std::string &hexstr)
{
	int value = 0;
	for (std::string::const_iterator it = hexstr.begin(); it != hexstr.end(); ++it)
	{
		char c = *it;
		value *= 16;
		if (c >= '0' && c <= '9')
			value += c - '0';
		else if (c >= 'a' && c <= 'f')
			value += c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			value += c - 'A' + 10;
		else
			return -1;
	}
	return value;
}

std::string Request::getRawRequest() const
{
	return rawRequest;
}

void Request::setRawRequest(const std::string &r)
{
	rawRequest = r;
}

bool processChunkedBody(std::string &raw_request, std::string &body)
{
	size_t pos = 0;

	while (true)
	{
		size_t chunk_size_end = raw_request.find("\r\n", pos);
		if (chunk_size_end == std::string::npos)
			return false;
		std::string chunk_size_hex = raw_request.substr(pos, chunk_size_end - pos);
		int chunk_size = hexstr_to_int(chunk_size_hex);
		pos = chunk_size_end + 2;
		if (chunk_size == 0)
			break;
		if (raw_request.size() < pos + chunk_size + 2)
			return false;
		body.append(raw_request, pos, chunk_size);
		pos += chunk_size + 2;
	}
	return true;
}

bool Request::isComplete(const std::string &raw_request) const
{
	size_t header_end = raw_request.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		std::cout << "header_end == std::string::npos" << std::endl;
		return false;
	}
	std::string body_part = raw_request.substr(header_end + 4);
	std::string content_length_str = getHeader("Content-Length");

	if (!content_length_str.empty())
	{
		int content_length;
		std::stringstream content_length_stream(content_length_str);
		content_length_stream >> content_length;
		std::string referer = getHeader("Referer");
		std::cout << "Content-Length: " << content_length << std::endl;
		std::cout << "Body size: " << body_part.size() << std::endl;
		if (raw_request.size() >= header_end + 4 + content_length)
		{
			std::cout << COLORIZE("raw_request.size() >= header_end + 4 + content_length", GREEN) << std::endl;
			return true;
		}
		else if (!referer.empty())
		{
			std::cout << COLORIZE("getHeader(\"referer\")", GREEN) << std::endl;
			return true;
		}
		std::cout << COLORIZE("raw_request.size() < header_end + 4 + content_length", RED) << std::endl;
	}
	else if (getHeader("Transfer-Encoding") == "chunked")
	{
		std::string temp_body;
		if (processChunkedBody(body_part, temp_body))
		{
			std::cout << "processChunkedBody(body_part, temp_body) == true" << std::endl;
			return true;
		}
		std::cout << "processChunkedBody(body_part, temp_body) == false" << std::endl;
		return false;
	}
	else if (method == "GET")
	{
		std::cout << "method == GET" << std::endl;
		return true;
	}
	else if (body_part.empty())
	{
		std::cout << "body_part.empty()" << std::endl;
		return true;
	}

	return false;
}

// Getters
std::string Request::getMethod() const
{
	return method;
}

std::string Request::getUri() const
{
	return uri;
}

bool Request::isRequestValid() const
{
	return requestIsValid;
}

// Setters
void Request::setMethod(const std::string &m)
{
	method = m;
}

void Request::setUri(const std::string &u)
{
	uri = u;
}

bool Request::parseStartLine(const std::string &start_line)
{
	std::istringstream stream(start_line);
	stream >> method >> uri >> http_version;

	if(method.empty() || uri.empty() || http_version.empty())
	{
		std::cout << "method.empty() || uri.empty() || http_version.empty()" << std::endl;
		return false;
	}
	return true;
}

bool Request::parseHeaders(const std::string &header_part)
{
	std::istringstream stream(header_part);
	std::string line;
	while (std::getline(stream, line) && line != "\r\n")
	{
		size_t pos = line.find(": ");
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			headers[key] = value;
		}
	}
	return !headers.empty();
}

bool Request::validateContentLength()
{
	if (headers.find("Content-Length") != headers.end())
	{
		std::istringstream content_length_stream(headers["Content-Length"]);
		size_t content_length;
		content_length_stream >> content_length;
		std::cout << "Content-Length: " << content_length << std::endl;
		std::cout << "Body size: " << body.size() << std::endl;
		return body.size() == content_length;
	}
	return true;
}

bool Request::keepAlive() const
{
	return headers.find("Connection") != headers.end() && headers.at("Connection") == "keep-alive";
}