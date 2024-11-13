#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Response.hpp"

class PostResponse : public Response {
	private:
		std::string _postData;
		std::string _responseData;
		std::string _filePath;
		stringMap _headersRequest;
		std::string _fileName;
		ServerConfigs _server;
		LocationConfigs _location;
		std::set<std::string> _validTypes;
		std::string _contentType;

	public:
		PostResponse(std::string filePath, std::string postData, ServerConfigs server, LocationConfigs location, stringMap headersRequest);
		void prepareResponse();

	private:
		int		_createFile();
		bool	_isValidContentTypeAndSetExtension();
		void	_removeBoundary();
};

#endif // POSTRESPONSE_HPP
