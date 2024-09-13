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
	response_stream << body;

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



Response responseTratament(Request &request, Logger &logger){
	Response response;
	std::ostringstream log;
	if(!request.requestIsValid){
		logger.logError("ERROR", "Error parsing request");
		std::string body = readFile("static/405.html");
		response.setStatus(405, "Method Not Allowed");
		response.setBody(body);
		return response;
	}
	std::string path = "static" + request.uri;
	if (path[path.size() - 1] == '/')
		path += "index.html";
	std::string body;
	struct stat status;
	if (stat(path.c_str(), &status) != 0)
	{
		logger.logError("ERROR", "File not found");
		body = readFile("static/404.html");
		response.setStatus(404, "Not Found");
		response.setBody(body);
		return response;
	}
	if(S_ISDIR(status.st_mode) && request.allow_directory_listing)
	{
		body = listDirectory(path);
		response.setStatus(200, "OK");
		response.setBody(body);
		return response;
	}
	if(access(path.c_str(), R_OK) != 0)
	{
		logger.logError("ERROR", "File not readable");
		body = readFile("static/403.html");
		response.setStatus(403, "Forbidden");
		response.setBody(body);
		return response;
	}
	body = readFile(path);
	response.setStatus(200, "OK");
	response.setBody(body);
	return response;
}