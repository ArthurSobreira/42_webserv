#include "Response.hpp"
#include "Utils.hpp"
#include <sys/stat.h>
#include "Config.hpp"

// Construtor padrão inicializando o status HTTP como 200 (OK)
Response::Response() : status_code(200), reason_phrase("OK") {}

// Gera a resposta HTTP completa
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

// Define o status da resposta
void Response::setStatus(int code, const std::string &reason)
{
	status_code = code;
	reason_phrase = reason;
}

// Define um cabeçalho na resposta
void Response::setHeader(const std::string &key, const std::string &value)
{
	headers[key] = value;
}

// Define o corpo da resposta e ajusta o Content-Length
void Response::setBody(const std::string &bodyContent)
{
	body = bodyContent;
	std::stringstream ss;
	ss << body.size();
	setHeader("Content-Length", ss.str());
}

// Métodos de acesso
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
	{
		return it->second;
	}
	return "";
}

std::string Response::getBody() const
{
	return body;
}

// Define o corpo e o tipo de conteúdo (Content-Type)
void Response::setBodyWithContentType(const std::string &bodyContent, const std::string &path)
{
	setBody(bodyContent);
	setHeader("Content-Type", getContentType(path));
}

void Response::processRequest(Request &request, Logger &logger)
{
	std::string path;
	if (request.getUri().find("static") != std::string::npos)
		path = request.getUri();
	else
		path = "static" + request.getUri();
	logger.logDebug(LOG_INFO, "'response' Request URI: " + path, true);
	if (path[path.size() - 1] == '/')
		path += "index.html";
	status Status;
	if (stat(path.c_str(), &Status) != 0)
	{
		handleError(404, "static/404.html", "File not found", logger);
		return;
	}
	if (S_ISDIR(Status.st_mode) && request.getIsAllowDirectoryListing())
	{
		std::string directoryListing = listDirectory(path);

		setBodyWithContentType(directoryListing, "directory.html");
		setStatus(200, "OK");
		return;
	}

	if (access(path.c_str(), R_OK) != 0)
	{
		handleError(403, "static/403.html", "File not readable", logger);
		return;
	}

	handleFileResponse(path, logger);
}

void Response::handleError(int status_code, const std::string &error_page, const std::string &error_message, Logger &logger)
{
	logger.logError("ERROR", error_message);
	std::string bodyContent = readFile(error_page);
	setStatus(status_code, error_message);
	setBodyWithContentType(bodyContent, error_page);
}

void Response::handleFileResponse(const std::string &path, Logger &logger)
{
	std::string bodyContent = readFile(path);
	setStatus(200, "OK");
	setBodyWithContentType(bodyContent, path);
	logger.logDebug(LOG_INFO, "Response body: " + bodyContent);
}
