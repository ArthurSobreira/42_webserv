#include "Response.hpp"
#include "Utils.hpp"
#include <sys/stat.h>
#include "Config.hpp"

Response::Response() : status_code(200), reason_phrase("OK") {}

std::string Response::generateResponse() const
{
	std::ostringstream response_stream;

	response_stream << "HTTP/1.1 " << status_code << " " << reason_phrase << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		response_stream << it->first << ": " << it->second << "\r\n";
	}
	response_stream << "\r\n";
	response_stream << this->body;

	return response_stream.str();
}

void Response::setStatus(int code, const std::string &reason)
{
	status_code = code;
	reason_phrase = reason;
}

void Response::setHeader(const std::string &key, const std::string &value)
{
	headers[key] = value;
}

void Response::setBody(const std::string &bodyContent)
{
	body = bodyContent;
	std::stringstream ss;
	ss << body.size();
	setHeader("Content-Length", ss.str());
}

int Response::getStatusCode() const
{
	return status_code;
}

std::string Response::getReasonPhrase() const
{
	return reason_phrase;
}

std::string Response::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end())
		return it->second;
	return "";
}

std::string Response::getBody() const
{
	return body;
}

void Response::setBodyWithContentType(const std::string &bodyContent, const std::string &path)
{
	setBody(bodyContent);
	setHeader("Content-Type", getContentType(path));
}

void Response::handlerValidRequest(Request &request, Logger &logger)
{
	if (!request.getIsRequestValid())
	{
		if (!request.validateMethod() && !request.getMethod().empty())
		{
			std::cout << "debbug response 9" << std::endl;
			std::cout << request.getRawRequest() << std::endl;
			handleError(405, "static/errors/405.html", "Method not allowed", logger);
			return;
		}
		std::cout << "debbug response 10" << std::endl;
		handleError(400, "static/errors/400.html", "Bad request", logger);
		return;
	}
	std::cout << "debbug response 11" << std::endl;
}

void Response::processRequest(Request &request, Logger &logger)
{
	std::string path;
	if (!request.getIsRequestValid())
	{
		std::cout << "debbug response 1" << std::endl;
		handlerValidRequest(request, logger);
		return;
	}

	if (request.getUri().find("static") != std::string::npos)
	{
		std::cout << "debbug response 2" << std::endl;
		path = "./" + request.getUri();
	}
	else
	{
		std::cout << "debbug response 3" << std::endl;
		path = "static" + request.getUri();
	}
	logger.logDebug(LOG_INFO, "'response' Request URI: " + path, true);
	if (path[path.size() - 1] == '/')
	{
		std::cout << "debbug response 4" << std::endl;
		path += "index.html";
	}
	status Status;
	if (stat(path.c_str(), &Status) != 0)
	{
		std::cout << "debbug response 5" << std::endl;
		handleError(404, "static/errors/404.html", "File not found", logger);
		return;
	}
	if (S_ISDIR(Status.st_mode) && request.getIsAllowDirectoryListing())
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
		handleError(403, "static/errors/403.html", "File not readable", logger);
		return;
	}
	std::cout << "debbug response 8" << std::endl;
	handleFileResponse(path, logger);
}

void Response::handleError(int status_code, const std::string &error_page, const std::string &error_message, Logger &logger)
{
	logger.logError("ERROR", error_message);
	std::string bodyContent = readFile(error_page);
	setStatus(status_code, error_message);
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
