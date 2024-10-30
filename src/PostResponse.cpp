#include "PostResponse.hpp"

PostResponse::PostResponse(std::string filePath, std::string postData, 
	ServerConfigs &server, LocationConfigs &location, std::string fileDescription) 
	: Response(), _postData(postData), _filePath(filePath), 
	_fileDescription(fileDescription), _server(server), _location(location) {}

std::string PostResponse::prepareResponse()
{
	
	_filePath = _location.uploadPath + "/" + _filePath;
	if(_postData.size() > _server.limitBodySize){
		handleError("413", _server.errorPages.at("413"), "Request Entity Too Large", _logger);
	}
	else if (!_location.uploadEnabled){
		handleError("405", _server.errorPages.at("405"), "Method Not Allowed", _logger);
	}
	else if(createFile() == 0){
		_statusCode = "201";
		_reasonPhrase = "Created";
		_body = "File created successfully";
	}
	else{
		handleError("500", _server.errorPages.at("500"), "Internal server error", _logger);
	}
	std::stringstream ss;
	ss << _body.size();
	_headers["Content-Length"] = ss.str();
	_headers["Content-Type"] = "text/html";
	return generateResponse();
}

int PostResponse::createFile()
{
	if (_fileDescription.empty())
	{
		_fileDescription = "default";
	}


	std::ofstream file(_filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!file.is_open())
	{
		return -1;
	}
	file.write(_postData.c_str(), _postData.size());
	file.close();
	return 0;
}