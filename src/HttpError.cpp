#include "HttpError.hpp"
#include "includes.hpp"

HttpError::HttpError(int status_code, const std::string &custom_message)
	: _status_code(status_code), _custom_message(custom_message)
{
	_initStatusMessages();
}

HttpError::HttpError(const HttpError &other)
	: _status_code(other._status_code), _custom_message(other._custom_message), _status_messages(other._status_messages){}

HttpError &HttpError::operator=(const HttpError &other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_custom_message = other._custom_message;
		_status_messages = other._status_messages;
	}
	return *this;
}

HttpError::~HttpError(){}

void HttpError::_initStatusMessages()
{
	_status_messages[200] = "OK";
	_status_messages[301] = "Moved Permanently";
	_status_messages[302] = "Found";
	_status_messages[400] = "Bad Request";
	_status_messages[401] = "Unauthorized";
	_status_messages[403] = "Forbidden";
	_status_messages[404] = "Not Found";
	_status_messages[405] = "Method Not Allowed";
	_status_messages[408] = "Request Timeout";
	_status_messages[429] = "Too Many Requests";
	_status_messages[500] = "Internal Server Error";
	_status_messages[501] = "Not Implemented";
	_status_messages[502] = "Bad Gateway";
	_status_messages[503] = "Service Unavailable";
	_status_messages[504] = "Gateway Timeout";
	_status_messages[505] = "HTTP Version Not Supported";
}

std::string HttpError::getBody() const
{
    std::ostringstream body, ssc;
	ssc << _status_code;
	std::string path = "static/" + ssc.str() + ".html";
    std::ifstream file(path.c_str());

    if (file.is_open())
    {
        body << file.rdbuf(); 
        file.close();
    }
    else
    {
        body << "<html><head><title>" + _status_messages.at(_status_code) + "</title></head>";
        body << "<body><h1>" + ssc.str() + " " + _status_messages.at(_status_code) + "</h1>";
        body << "<p>" + (_custom_message.empty() ? _status_messages.at(_status_code) : _custom_message) + "</p>";
        body << "</body></html>";
    }
    return body.str();
}