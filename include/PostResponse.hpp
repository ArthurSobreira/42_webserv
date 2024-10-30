#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include "Response.hpp"
#include <string>

class PostResponse : public Response {
    private:
        std::string _postData;
        std::string _responseData;
		std::string _filePath;
		std::string _fileDescription;
		std::string _fileName;
		ServerConfigs _server;
		LocationConfigs _location;

    public:
		PostResponse(std::string filePath, std::string postData, 
			ServerConfigs &server, LocationConfigs &location, 
			std::string fileDescription="");

        std::string prepareResponse();
		int createFile();
};

#endif // POSTRESPONSE_HPP
