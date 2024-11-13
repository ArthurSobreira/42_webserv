#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Server.hpp"

class Response {
	protected:
		stringMap _headers;
		std::string _body;
		std::string _statusCode;
		std::string _reasonPhrase;
		Logger	_logger;

	public:
		Response( void );
		virtual ~Response( void );
		std::string generateResponse( void ) const;

		// utility functions
		void handleError(std::string _statusCode, const std::string &error_page, const std::string &error_message, Logger &logger);
		void handleFileResponse(const std::string &path, Logger &logger);
};

#endif // RESPONSE_HPP
