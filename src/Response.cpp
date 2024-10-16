#include "Response.hpp"
#include "Utils.hpp"
#include "Includes.hpp"
#include "Config.hpp"

Response::Response() : _status_code(200), _reason_phrase("OK"), _root("static") {}

std::string Response::generateResponse() const
{
	std::ostringstream response_stream;

	response_stream << "HTTP/1.1 " << _status_code << " " << _reason_phrase << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		response_stream << it->first << ": " << it->second << "\r\n";
	}
	response_stream << "\r\n";
	response_stream << body;

	return response_stream.str();
}

void Response::setStatus(int code, const std::string &reason)
{
	_status_code = code;
	_reason_phrase = reason;
}

int Response::getStatusCode() const
{
	return _status_code;
}

std::string Response::getReasonPhrase() const
{
	return _reason_phrase;
}

void Response::setBodyWithContentType(const std::string &bodyContent, const std::string &path)
{
	setBody(bodyContent);
	setHeader("Content-Type", getContentType(path));
}

// void Response::handlerValidRequest(Request &request, Logger &logger)
// {
// 	if (!request.isRequestValid())
// 	{
// 		if (!request.validateMethod() && !request.getMethod().empty())
// 		{
// 			std::cout << "debbug response 9" << std::endl;
// 			handleError(405, "static/errors/405.html", "Method not allowed", logger);
// 			return;
// 		}
// 		std::cout << "debbug response 10" << std::endl;
// 		handleError(400, "static/errors/400.html", "Bad request", logger);
// 		return;
// 	}
// 	std::cout << "debbug response 11" << std::endl;
// }

bool isRepeatedMethod(const std::vector<httpMethod> &methodsVector, httpMethod method)
{
	for (std::vector<httpMethod>::const_iterator it = methodsVector.begin(); it != methodsVector.end(); ++it)
	{
		if (*it == method)
			return true;
	}
	return false;
}

httpMethod returnMethod(std::string method)
{
	if (method == "GET")
		return GET;
	if (method == "POST")
		return POST;
	if (method == "DELETE")
		return DELETE;
	return INVALID;
}

bool Response::validMethod(const LocationConfigs it, const std::string &method)
{

	if (isRepeatedMethod(it.methods, returnMethod(method)))
		return true;
	return false;
}

LocationConfigs Response::returnLocationConfig(const ServerConfigs *respconfig, const std::string &uri)
{
	LocationConfigs bestMatch;
	size_t bestMatchLength = 0;

	for (std::vector<LocationConfigs>::const_iterator it = respconfig->locations.begin(); it != respconfig->locations.end(); ++it)
	{
		if (uri.find(it->locationPath) == 0 && it->locationPath.length() > bestMatchLength)
		{
			bestMatch = *it;
			bestMatchLength = it->locationPath.length();
		}
	}

	return bestMatch;
}

bool isValidContentType(const std::string &contentType)
{
	// Lista de Content-Types permitidos
	std::set<std::string> validTypes;
	validTypes.insert("image/jpeg");
	validTypes.insert("image/png");
	validTypes.insert("image/gif");
	validTypes.insert("audio/mpeg");
	validTypes.insert("audio/wav");
	validTypes.insert("application/pdf");
	validTypes.insert("text/plain");
	validTypes.insert("text/html");
	validTypes.insert("text/css");
	validTypes.insert("text/javascript");
	return validTypes.find(contentType) != validTypes.end();
}

void Response::postHandler(std::string path, const LocationConfigs &location, const Request &request, const ServerConfigs *respconfig, Logger &logger)
{
	(void)path;
	std::string contentType = request.getHeader("Content-Type");
	if (!isValidContentType(contentType))
	{
		handleError(415, respconfig->errorPages.at("415"), "Unsupported Media Type", logger);
		return;
	}
	std::string pathl = location.uploadPath;
	pathl += "teste.png";

	std::ofstream outFile(pathl.c_str(), std::ios::out | std::ios::trunc);
	if (!outFile)
	{
		handleError(500, respconfig->errorPages.at("500"), "Internal Server Error", logger);
		return;
	}

	outFile << request.getBody();
	outFile.close();

	// Configurar a resposta de sucesso
	setStatus(201, "Created");
	setBodyWithContentType("Resource created successfully", "text/plain");
	logger.logDebug(LOG_INFO, "Resource created at: " + path);
}

void Response::getHandler(std::string path, const LocationConfigs &location, const ServerConfigs *respconfig, Logger &logger)
{
	status Status;

	if (isDirectory(path) && path[path.size() - 1] != '/')
		path += "/";
	if (path[path.size() - 1] == '/' && !location.autoindex)
	{
		std::cout << "debbug response 4" << std::endl;
		path += "index.html";
	}
	if (stat(path.c_str(), &Status) != 0)
	{
		std::cout << "debbug response 5" << std::endl;
		handleError(404, respconfig->errorPages.at("404"), "File not found", logger);
		return;
	}
	if (S_ISDIR(Status.st_mode))
	{
		std::cout << "debbug response 6" << std::endl;
		std::string directoryListing = listDirectory(path);
		setBodyWithContentType(directoryListing, "directory.html");
		setStatus(200, "OK");
		return;
	}
	if (access(path.c_str(), R_OK) != 0)
	{
		std::cout << "debbug response 7" << std::endl;
		handleError(403, respconfig->errorPages.at("403"), "File not readable", logger);
		return;
	}
	std::cout << "debbug response 8" << std::endl;
	handleFileResponse(path, logger);
}

void Response::processRequest(Request &request, const ServerConfigs *respconfig, Logger &logger)
{
	LocationConfigs location = returnLocationConfig(respconfig, request.getUri());
	std::string path = location.root + request.getUri();
	if (!validMethod(location, request.getMethod()))
	{
		std::cout << "debbug response 1" << std::endl;
		handleError(405, respconfig->errorPages.at("405"), "Method not allowed", logger);
		return;
	}
	switch (returnMethod(request.getMethod()))
	{
	case GET:
		std::cout << "debbug response 2" << std::endl;
		getHandler(path, location, respconfig, logger);
		break;
	case POST:
		std::cout << "debbug response 3" << std::endl;
		postHandler(path, location, request, respconfig, logger);
		break;
	case DELETE:
		std::cout << "debbug response 4" << std::endl;
		break;
	default:
		handleError(400, respconfig->errorPages.at("400"), "Bad Request", logger);
		break;
	}
	logger.logDebug(LOG_INFO, "'response' Request URI: " + path, true);
}

void Response::handleError(int _status_code, const std::string &error_page, const std::string &error_message, Logger &logger)
{
	logger.logError(LOG_ERROR, error_message);
	std::string bodyContent = readFile(error_page);
	setStatus(_status_code, error_message);
	setBodyWithContentType(bodyContent, error_page);
	std::stringstream ss;
	ss << bodyContent.size();
	setHeader("Content-Length", ss.str());
	setHeader("Content-Type", "text/html");
}

void Response::handleFileResponse(const std::string &path, Logger &logger)
{
	std::string bodyContent = readFile(path);
	setStatus(200, "OK");
	setBodyWithContentType(bodyContent, path);
	logger.logDebug(LOG_INFO, "Response body: " + bodyContent);
}
