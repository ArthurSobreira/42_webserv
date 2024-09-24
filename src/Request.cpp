#include "Request.hpp"

bool Request::parseRequest(const std::string &raw_request)
{
	std::istringstream stream(raw_request);
	stream >> method >> uri >> http_version;
	this->allow_directory_listing = true;
	if (method.empty() || uri.empty() || http_version.empty())
	{
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
			// std::cout << "["<< key <<"]" << std::endl;
			// std::cout << "["<< value <<"]" << std::endl;
		}
	}
	// std::cout << "["<< headers["accept"] <<"]" << std::endl;

	return true;
}
Request::Request() : method(""), uri(""), http_version("") {}
