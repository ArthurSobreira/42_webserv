#include "GetResponse.hpp"

GetResponse::GetResponse(std::string filePath) : Response(), _filePath(filePath) {}

void GetResponse::listDirectoryHandler(){
		std::cout << "debbug response 6" << std::endl;
		_body = "<html><head><title>Index of " + _filePath + "</title></head><body><h1>Index of " + _filePath + "</h1><hr><pre>";
		// _body = listDirectory(_filePath);
		_headers["Content-Type"] = "text/html";
		std::stringstream ss;
		ss << _body.size();
		_headers["Content-Length"] = ss.str();
		_reasonPhrase = "OK";
		_statusCode = "200";
}

void GetResponse::prepareResponse(const LocationConfigs &location, const ServerConfigs &respconfig){
	status Status;
	std::cout << RED << "prepareResponse [" <<_filePath << "]"<< RESET << std::endl;
	_filePath = location.root + _filePath;
	std::cout << _filePath << std::endl;
	if (_filePath[_filePath.size() - 1] == '/' && !location.autoindex)
	{
		std::cout << "debbug response 4" << std::endl;
		_filePath += location.index;
	}
	if (stat(_filePath.c_str(), &Status) != 0)
	{
		std::cout << "debbug response 5" << std::endl;
		handleError("404", respconfig.errorPages.at("404"), "File not found", _logger);
		return;
	}
	if (S_ISDIR(Status.st_mode))
	{
		listDirectoryHandler();
		return;
	}
	if (access(_filePath.c_str(), R_OK) != 0)
	{
		std::cout << "debbug response 7" << std::endl;
		handleError("403", respconfig.errorPages.at("403"), "File not readable", _logger);
		return;
	}
	std::cout << "debbug response 8" << std::endl;
	handleFileResponse(_filePath, _logger);
}
