#include "PostResponse.hpp"

/* Constructor Method */
PostResponse::PostResponse( std::string filePath, std::string postData, 
	stringMap headersRequest, LocationConfigs location )
	: Response(), _filePath(filePath), _postData(postData), 
	_headersRequest(headersRequest), _location(location) {
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

/* Destructor Method */
PostResponse::~PostResponse( void ) {};

/* Public Method */
void	PostResponse::prepareResponse( void ) {
	_filePath = _location.uploadPath + "/" + _filePath;
	if (_postData.size() > _location.server->limitBodySize)
		handleError("413", _location.server->errorPages.at("413"), ERROR_TOO_LARGE);
	else if (!_location.uploadEnabled)
		handleError("405", _location.server->errorPages.at("405"), ERROR_METHOD_NOT_ALLOWED);
	else {
		int valid = _createFile();
		if (valid == SUCCESS) {
			if (access(_filePath.c_str(), F_OK) != -1) {
				logger.logDebug(LOG_INFO, "File Created: " + _filePath, true);
				_statusCode = "201";
				_reasonPhrase = "Created";
				_body = "<html><head><title>Created</title></head><body><h1>Created</h1></body></html>";
				std::stringstream ss;
				ss << _body.size();
				_headers["Content-Length"] = ss.str();
				_headers["Content-Type"] = "text/html";
			}
			else
				handleError("500", _location.server->errorPages.at("500"), ERROR_INTERNAL_SERVER);
		}
		else if (valid == BAD_REQUEST)
			handleError("400", _location.server->errorPages.at("400"), ERROR_BAD_REQUEST);
		else if (valid == UNSUPPORTED_MEDIA_TYPE)
			handleError("415", _location.server->errorPages.at("415"), ERROR_UNSUPPORTED_MEDIA_TYPE);
		else if (valid == INTERNAL_SERVER_ERROR) {
			logger.logError(LOG_ERROR, "Error creating file: " + _filePath, true);
			handleError("500", _location.server->errorPages.at("500"), ERROR_INTERNAL_SERVER);
		}
	}
}

/* Private Methods */
int	PostResponse::_createFile( void ) {
	static int archivo = 0;
	if (_postData.empty()) {
		return BAD_REQUEST;
	}
	if (!_headersRequest["boundary"].empty()) {
		_fileName = _headersRequest["filename"];
		_removeBoundary();
	}
	else {
		if (_isValidContentTypeAndSetExtension()) {
			std::stringstream ss;
			ss << archivo++;
			_fileName = "file" + ss.str() + "." + _contentType;
		}
		else {
			return UNSUPPORTED_MEDIA_TYPE;
		}
	}
	_filePath = _location.uploadPath + "/" + _fileName;
	std::ofstream file(_filePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!file.is_open()) {
		return INTERNAL_SERVER_ERROR;
	}
	file.write(_postData.c_str(), _postData.size());
	file.close();
	return SUCCESS;
}

bool PostResponse::_isValidContentTypeAndSetExtension( void ) {
	if (_validTypes.find(_headersRequest["Content-Type"]) == _validTypes.end()) {
		return false;
	}
	_contentType = _headersRequest["Content-Type"];
	_contentType = _contentType.substr(_contentType.find("/") + 1);
	if (_contentType == "plain")
		_contentType = "txt";
	if (_contentType == "javascript")
		_contentType = "js";
	return true;
}

void	PostResponse::_removeCarriageReturn( void ) {
	std::string str = _headersRequest["boundary"];
	size_t pos = str.find("\r");
	while (pos != std::string::npos) {
		str.erase(pos, 1);
		pos = str.find("\r");
	}
}

void PostResponse::_removeBoundary( void ) {
	_removeCarriageReturn();
	size_t pos = _postData.find("\r\n\r\n");
	_postData = _postData.substr(pos + 4);
	pos = _postData.find(_headersRequest["boundary"]);
	_postData = _postData.substr(0, pos - 2);
}
