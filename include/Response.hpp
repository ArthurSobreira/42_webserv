#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Includes.hpp"
#include "Logger.hpp"
#include "Server.hpp"

class Response {
    protected:
        int _clientSocket;
        Logger &_logger;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _status_code;
		std::string _reason_phrase;

    public:
        Response(int clientSocket, Logger &logger);
		virtual ~Response();
		std::string generateResponse() const;
		// utility functions
		void handleError(std::string _status_code, const std::string &error_page, const std::string &error_message, Logger &logger);
		void handleFileResponse(const std::string &path, Logger &logger);
       	virtual void prepareResponse(const LocationConfigs &location, const ServerConfigs &respconfig) = 0;
};

#endif // RESPONSE_HPP
