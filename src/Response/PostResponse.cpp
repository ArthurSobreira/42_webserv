#include "PostResponse.hpp"

PostResponse::PostResponse(std::string filePath, std::string postData, ServerConfigs server, LocationConfigs location, stringMap headersRequest) : Response(), _postData(postData), _filePath(filePath), _headersRequest(headersRequest), _server(server), _location(location)
{
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

bool PostResponse::_isValidContentTypeAndSetExtension()
{
	if (_validTypes.find(_headersRequest["Content-Type"]) == _validTypes.end())
		return false;
	_contentType = _headersRequest["Content-Type"];
	_contentType = _contentType.substr(_contentType.find("/") + 1);
	if (_contentType == "plain")
		_contentType = "txt";
	if (_contentType == "javascript")
		_contentType = "js";
	return true;
}

void PostResponse::prepareResponse()
{
	_filePath = _location.uploadPath + "/" + _filePath;
	if (_postData.size() > _server.limitBodySize)
		handleError("413", _server.errorPages.at("413"), ERROR_TOO_LARGE, _logger);
	else if (!_location.uploadEnabled)
		handleError("405", _server.errorPages.at("405"), ERROR_METHOD_NOT_ALLOWED, _logger);
	else
	{
		int valid = _createFile();
		if (valid == SUCCESS)
		{
			if (access(_filePath.c_str(), F_OK) != -1)
			{
				_statusCode = "201";
				_reasonPhrase = "Created";
				_body = "<html><head><title>Created</title></head><body><h1>Created</h1></body></html>";
				std::stringstream ss;
				ss << _body.size();
				_headers["Content-Length"] = ss.str();
				_headers["Content-Type"] = "text/html";
			}
			else
				handleError("500", _server.errorPages.at("500"), ERROR_INTERNAL_SERVER, _logger);
		}
		else if (valid == BAD_REQUEST)
			handleError("400", _server.errorPages.at("400"), ERROR_BAD_REQUEST, _logger);
		else if (valid == UNSUPPORTED_MEDIA_TYPE)
			handleError("415", _server.errorPages.at("415"), ERROR_UNSUPPORTED_MEDIA_TYPE, _logger);
		else if (valid == INTERNAL_SERVER_ERROR)
			handleError("500", _server.errorPages.at("500"), ERROR_INTERNAL_SERVER, _logger);
	}
}

static void removeCarriageReturn(std::string &str)
{
	size_t pos = str.find("\r");
	while (pos != std::string::npos)
	{
		str.erase(pos, 1);
		pos = str.find("\r");
	}
}

void PostResponse::_removeBoundary()
{
	removeCarriageReturn(_headersRequest["boundary"]);
	size_t pos = _postData.find("\r\n\r\n");
	_postData = _postData.substr(pos + 4);
	pos = _postData.find(_headersRequest["boundary"]);
	_postData = _postData.substr(0, pos - 2);
}

int PostResponse::_createFile()
{
	static int archivo = 0;
	if (_postData.empty())
		return BAD_REQUEST;
	if (!_headersRequest["boundary"].empty())
	{
		_fileName = _headersRequest["filename"];
		_removeBoundary();
	}
	else
	{
		if (_isValidContentTypeAndSetExtension())
		{
			std::stringstream ss;
			ss << archivo++;
			_fileName = "file" + ss.str() + "." + _contentType;
		}
		else
			return UNSUPPORTED_MEDIA_TYPE;
	}
	_filePath = _location.uploadPath + "/" + _fileName;
	std::ofstream file(_filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!file.is_open())
		return INTERNAL_SERVER_ERROR;
	file.write(_postData.c_str(), _postData.size());
	file.close();
	return SUCCESS;
}
