#include "PostResponse.hpp"

PostResponse::PostResponse(std::string filePath, std::string postData, ServerConfigs server, LocationConfigs location, std::map<std::string,std::string> headers) : Response(), _postData(postData), _filePath(filePath), _headers(headers), _server(server), _location(location){
	_validTypes.insert("image/jpeg");
	_validTypes.insert("image/png");
	_validTypes.insert("image/gif");
	_validTypes.insert("audio/mpeg");
	_validTypes.insert("audio/wav");
	_validTypes.insert("application/pdf");
	_validTypes.insert("text/plain");
	_validTypes.insert("text/html");
	_validTypes.insert("text/css");
	_validTypes.insert("text/javascript");
	_validTypes.insert("video/mp4");
}


bool PostResponse::isValidContentTypeAndSetExtension()
{
	if (_validTypes.find(_headers["Content-Type"]) == _validTypes.end())
	{
		return false;
	}
	_contentType = _headers["Content-Type"];
	_contentType = _contentType.substr(_contentType.find("/") + 1);
	if (_contentType == "plain")
		_contentType = "txt";
	if (_contentType == "javascript")
		_contentType = "js";
	return true;
}



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
		_body = "<html><h1>File uploaded successfully</h1></html>";
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

void PostResponse::removeBoundary(){}

int PostResponse::createFile()
{
	static int archivo = 0;
	if (!_headers["boundary"].empty())
	{
		_fileName = _headers["filename"];
		// removeBoundary();
	}
	else
	{
		isValidContentTypeAndSetExtension();
		std::stringstream ss;
		ss << archivo++;
		_fileName = "file" + ss.str() + "." + _contentType;
	}

	_filePath = _location.uploadPath + "/" + _fileName;


	std::ofstream file(_filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!file.is_open())
	{
		return -1;
	}
	file.write(_postData.c_str(), _postData.size());
	file.close();
	return 0;
}