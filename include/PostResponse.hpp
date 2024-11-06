#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include "Response.hpp"
#include <string>

class PostResponse : public Response
{
private:
	std::string _postData;
	std::string _responseData;
	std::string _filePath;
	std::map<std::string, std::string> _headersRequest;
	std::string _fileName;
	ServerConfigs _server;
	LocationConfigs _location;
	std::set<std::string> _validTypes;
	std::string _contentType;

public:
	PostResponse(std::string filePath, std::string postData, ServerConfigs server, LocationConfigs location, std::map<std::string, std::string> headersRequest);
	void prepareResponse();

private:
	int createFile();
	bool isValidContentTypeAndSetExtension();
	void removeBoundary();
};

#endif // POSTRESPONSE_HPP
