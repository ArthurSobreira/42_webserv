#include "Includes.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

Response::Response() : status_code(200), reason_phrase("OK") {}

std::string Response::generateResponse() const {
	std::ostringstream response_stream;

	response_stream << "HTTP/1.1 " << status_code << " " << reason_phrase << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		response_stream << it->first << ": " << it->second << "\r\n";
	}
	response_stream << "\r\n";
	std::cout << response_stream.str() << std::endl;
	response_stream << this->body;

	return response_stream.str();
}

void Response::setStatus(int code, const std::string &reason) {
	status_code = code;
	reason_phrase = reason;
}

void Response::setHeader(const std::string &key, const std::string &value) {
	headers[key] = value;
}

void Response::setBody(const std::string &bodyContent) {
	body = bodyContent;
	std::stringstream ss;
	ss << body.size();
	setHeader("Content-Length", ss.str());
}

int Response::getStatusCode() const {
	return status_code;
}

std::string Response::getReasonPhrase() {
	return reason_phrase;
}

std::string Response::getHeader(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end()) {
		return it->second;
	}
	return "";
}

std::string Response::getBody() const {
	return body;
}

void Response::responseTratament(Request &request, Logger &logger){
	std::ostringstream log;
	if(!request.getIsRequestValid()){
		logger.logError("ERROR", "Error parsing request");
		std::string bodyr = readFile("static/405.html");
		this->setStatus(405, "Method Not Allowed");
		setHeader("Content-Type", getContentType("static/405.html"));
		this->setBody(bodyr);
		return ;
	}
	std::string path = "static" + request.getUri();
	if (path[path.size() - 1] == '/')
		path += "index.html";
	std::string bodyr;
	struct stat status;
	if (stat(path.c_str(), &status) != 0)
	{
		logger.logError("ERROR", "File not found");
		bodyr = readFile("static/errors/404.html");
		this->setStatus(404, "Not Found");
		this->setBody(bodyr);
		this->setHeader("Content-Type", getContentType("static/errors/404.html"));
		return ;
	}
	if(S_ISDIR(status.st_mode) && request.getIsAllowDirectoryListing())
	{
		bodyr = listDirectory(path);
		this->setStatus(200, "OK");
		this->setBody(bodyr);
		this->setHeader("Content-Type", getContentType("static/errors/404.html"));
		return ;
	}
	if(access(path.c_str(), R_OK) != 0)
	{
		logger.logError("ERROR", "File not readable");
		bodyr = readFile("static/errors/403.html");
		this->setStatus(403, "Forbidden");
		this->setBody(bodyr);
		this->setHeader("Content-Type", getContentType("static/errors/403.html")); 
		return ;
	}
	bodyr = readFile(path);
	this->setStatus(200, "OK");
	this->setHeader("Content-Type", getContentType(path));
	setBody(bodyr);
}
