// #include "Response.hpp"
// #include "Utils.hpp"
// #include "Includes.hpp"
// #include "Config.hpp"

// Response::Response() : _status_code(200), _reason_phrase("OK"), _root("static"){}

// std::string Response::generateResponse() const
// {
// 	std::ostringstream response_stream;

// 	response_stream << "HTTP/1.1 " << _status_code << " " << _reason_phrase << "\r\n";
// 	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != _headers.end(); ++it)
// 	{
// 		response_stream << it->first << ": " << it->second << "\r\n";
// 	}
// 	response_stream << "\r\n";
// 	response_stream << body;

// 	return response_stream.str();
// }

// void Response::setStatus(int code, const std::string &reason)
// {
// 	_status_code = code;
// 	_reason_phrase = reason;
// }


// int Response::getStatusCode() const
// {
// 	return _status_code;
// }

// std::string Response::getReasonPhrase() const
// {
// 	return _reason_phrase;
// }

// void Response::setBodyWithContentType(const std::string &bodyContent, const std::string &path)
// {
// 	setBody(bodyContent);
// 	setHeader("Content-Type", getContentType(path));
// }

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
// bool isRepeatedMethod(const std::vector<httpMethod> &methodsVector, httpMethod method)
// {
// 	for (std::vector<httpMethod>::const_iterator it = methodsVector.begin(); it != methodsVector.end(); ++it)
// 	{
// 		if (*it == method)
// 			return true;
// 	}
// 	return false;
// }

// httpMethod returnMethod(std::string method)
// {
// 	if (method == "GET")
// 		return GET;
// 	if (method == "POST")
// 		return POST;
// 	if (method == "DELETE")
// 		return DELETE;
// 	return INVALID;
// }


// bool Response::validConfigWithRequest(const ServerConfigs* respconfig, const std::string &uri, const std::string &method)
// {
// 	if (respconfig->locations.empty())
// 		return true;
// 	for (std::vector<LocationConfigs>::const_iterator it = respconfig->locations.begin(); it != respconfig->locations.end(); ++it)
// 	{
// 		if (uri.find(it->locationPath) != std::string::npos)
// 		{
// 			if (isRepeatedMethod(it->methods, returnMethod(method)))
// 				return true;
// 		}
// 	}
// 	return false;


// }

// void Response::processRequest(Request &request, const ServerConfigs* respconfig,Logger &logger)
// {
// 	(void)respconfig;
// 	std::string path;
// 	if (!request.isRequestValid())
// 	{
// 		std::cout << "debbug response 1" << std::endl;
// 		handlerValidRequest(request, logger);
// 		return;
// 	}

// 	if (request.getUri().find(_root) != std::string::npos)
// 	{
// 		std::cout << "debbug response 2" << std::endl;
// 		path = "./" + request.getUri();
// 	}
// 	else
// 	{
// 		std::cout << "debbug response 3" << std::endl;
// 		path = _root + request.getUri();
// 	}
// 	logger.logDebug(LOG_INFO, "'response' Request URI: " + path, true);
// 	if (path[path.size() - 1] == '/')
// 	{
// 		std::cout << "debbug response 4" << std::endl;
// 		path += "index.html";
// 	}
// 	status Status;
// 	if (stat(path.c_str(), &Status) != 0)
// 	{
// 		std::cout << "debbug response 5" << std::endl;
// 		handleError(404, "static/errors/404.html", "File not found", logger);
// 		return;
// 	}
// 	if (S_ISDIR(Status.st_mode))
// 	{
// 		std::cout << "debbug response 6" << std::endl;
// 		std::string directoryListing = listDirectory(path);

// 		setBodyWithContentType(directoryListing, "directory.html");
// 		setStatus(200, "OK");
// 		return;
// 	}

// 	if (access(path.c_str(), R_OK) != 0)
// 	{
// 		std::cout << "debbug response 7" << std::endl;
// 		handleError(403, "static/errors/403.html", "File not readable", logger);
// 		return;
// 	}
// 	std::cout << "debbug response 8" << std::endl;
// 	handleFileResponse(path, logger);
// }

// void Response::handleError(int _status_code, const std::string &error_page, const std::string &error_message, Logger &logger)
// {
// 	logger.logError("ERROR", error_message);
// 	std::string bodyContent = readFile(error_page);
// 	setStatus(_status_code, error_message);
// 	setBodyWithContentType(bodyContent, error_page);
// 	std::stringstream ss;
// 	ss << bodyContent.size();
// 	setHeader("Content-Length", ss.str());
// 	setHeader("Content-Type", "text/html");
// }


// void Response::handleFileResponse(const std::string &path, Logger &logger)
// {
// 	std::string bodyContent = readFile(path);
// 	setStatus(200, "OK");
// 	setBodyWithContentType(bodyContent, path);
// 	logger.logDebug(LOG_INFO, "Response body: " + bodyContent);
// }
