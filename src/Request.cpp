#include "Request.hpp"
#include "Utils.hpp"
#include "Includes.hpp"

Request::Request(const std::string &rawRequest) : _rawRequest(rawRequest)
{
	_method = INVALID;
	_isCGI = false;
	_connectionClose = false;
	parseRequest();
}

httpMethod Request::getMethod() const { return _method; }

const std::string &Request::getUri() const { return _uri; }

const std::string &Request::getHeader(const std::string &name) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(name);
	if (it == _headers.end())
	{
		static const std::string empty;
		return empty;
	}
	return it->second;
}

const std::map<std::string, std::string> &Request::getHeaders() const { return _headers; }

const std::string &Request::getBody() const { return _body; }

void Request::parseRequest()
{
	std::istringstream requestStream(_rawRequest);
	std::string line;
	std::vector<std::string> headerLines;

	size_t pos = _rawRequest.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		// Separar cabeçalhos e corpo
		std::string headersPart = _rawRequest.substr(0, pos);
		_body = _rawRequest.substr(pos + 4);
		// Processar cabeçalhos linha por linha
		std::istringstream headersStream(headersPart);
		bool isFirstLine = true;
		while (std::getline(headersStream, line))
		{
			if (line.empty() || line == "\r")
				continue;

			if (isFirstLine)
			{
				parseMethodAndUri(line);
				isFirstLine = false;
			}
			else
			{
				headerLines.push_back(line);
			}
		}
		parseHeaders(headerLines);
	}

	parseBody(); // Verifica o Content-Length e exibe informações de tamanho
}

void Request::parseMethodAndUri(const std::string &line)
{
	std::istringstream lineStream(line);
	std::string method;

	lineStream >> method >> _uri >> _version;

	parserQueryString();
	_method = parseMethod(method);
	if (_uri != "/"){
		_uri = removeLastSlashes(_uri);
	}

	std::cout << "Method: " << _method << std::endl;
	std::cout << "URI: " << _uri << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Query string: " << _queryString << std::endl;
}

void Request::parseHeaders(const std::vector<std::string> &headerLines)
{
	for (std::vector<std::string>::const_iterator it = headerLines.begin(); it != headerLines.end(); ++it)
	{
		size_t colonPos = it->find(':');
		if (colonPos != std::string::npos)
		{
			std::string name = it->substr(0, colonPos);
			std::string value = it->substr(colonPos + 2);
			_headers[name] = value;
		}
	}
}

void Request::extractMultipartNamesAndFilenames()
{
	std::string boundary = "--" + _headers["boundary"];
	std::string endBoundary = boundary + "--";
	size_t pos = _body.find(boundary);
	size_t endPos = _body.find(endBoundary);
	while (pos != std::string::npos && pos < endPos)
	{
		size_t namePos = _body.find("name=\"", pos);
		size_t filenamePos = _body.find("filename=\"", pos);
		if (namePos != std::string::npos && filenamePos != std::string::npos)
		{
			namePos += 6;
			filenamePos += 10;
			size_t nameEnd = _body.find("\"", namePos);
			size_t filenameEnd = _body.find("\"", filenamePos);
			std::string name = _body.substr(namePos, nameEnd - namePos);
			std::string filename = _body.substr(filenamePos, filenameEnd - filenamePos);
			_headers["name"] = name;
			_headers["filename"] = filename;	
		}
		pos = _body.find(boundary, pos + 1);
	}
	std::cout << "name: " << _headers["name"] << std::endl;
	std::cout << "filename: " << _headers["filename"] << std::endl;
}

void Request::parseBody()
{
	if (_headers.find("Content-Length") == _headers.end())
	{
		return;
	}
	std::cout << RED << "Content-Length: " << _headers["Content-Length"] << RESET <<std::endl;
	std::cout << "Body size: " << _body.size() << std::endl;
	if (_headers.find("Content-Type") != _headers.end())
	{
		std::string contentType = _headers["Content-Type"];
		if (contentType.find("multipart/form-data") != std::string::npos)
		{
			size_t pos = contentType.find("boundary=");
			if (pos != std::string::npos)
			{
				_headers["boundary"] = contentType.substr(pos + 9);
				extractMultipartNamesAndFilenames();
				std::cout << YELLOW << "parseada" << RESET << std::endl;
			}
		}
	}
}

httpMethod Request::parseMethod(const std::string &method)
{
	if (method == "GET")
	{
		return GET;
	}
	else if (method == "POST")
	{
		return POST;
	}
	else if (method == "DELETE")
	{
		return DELETE;
	}
	return INVALID;
}

bool counterOneSlash(const std::string &uri)
{
	int counter = 0;
	for (size_t i = 0; i < uri.size(); i++)
	{
		if (uri[i] == '/')
		{
			counter++;
		}
	}
	if (counter == 1 && uri[0] == '/')
	{
		return true;
	}
	return false;
}

std::string Request::folderPath()
{
	if (_uri == "/")
		return _uri;
	std::string folderPath = _uri;
	if (isDirectory(folderPath) && folderPath[folderPath.size() - 1] != '/')
	{
		folderPath += "/";
	}
	if (counterOneSlash(folderPath))
	{
		folderPath = "/";
		return folderPath;
	}
	if (folderPath[folderPath.size() - 1] == '/')
	{
		folderPath = folderPath.substr(0, folderPath.size() - 1);
	}
	size_t pos = folderPath.find_last_of('/');
	if (pos != std::string::npos)
	{
		folderPath = folderPath.substr(0, pos);
	}
	return folderPath;
}

std::string Request::validateRequest(Config _config, ServerConfigs server)
{
	static int counter = 0;
	std::string error = "";
	bool locationFound = false;
	std::cout << "passou aki " << counter++ << std::endl;
	_location = _config.getLocationConfig(server, _uri, locationFound);
	if (!locationFound)
	{
		_location = _config.getLocationConfig(server, folderPath(), locationFound);
	}
	if (!locationFound)
	{
		error = "404";
	}
	if (std::find(_location.methods.begin(), _location.methods.end(), getMethod()) == _location.methods.end())
	{
		error = "405";
	}
	if (_location.cgiEnabled)
	{
		_isCGI = true;
	}
	return error;
}

void Request::checkConnectionClose()
{
	std::map<std::string, std::string>::const_iterator it = _headers.find("Connection");
	if (it != _headers.end() && it->second == "close")
	{
		_connectionClose = true;
	}
}


void Request::parserQueryString()
{
	size_t pos = _uri.find("?");
	if (pos != std::string::npos)
	{
		_queryString = _uri.substr(pos + 1);
		_uri = _uri.substr(0, pos);
	}
}

std::string Request::getQueryString() const
{
	return _queryString;
}
