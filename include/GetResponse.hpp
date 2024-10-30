#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "Response.hpp"
#include "Logger.hpp"
#include "Includes.hpp"
class GetResponse : public Response {
	private:
		std::string _filePath;
		std::map<std::string, std::string> _headers;

	public:
		GetResponse(int clientSocket, Logger *logger, std::string filePath);
		void listDirectoryHandler();
		void prepareResponse(const LocationConfigs &location, const ServerConfigs &respconfig);
};

#endif // GETRESPONSE_HPP
