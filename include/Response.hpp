#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Server.hpp"

class Response {
	protected:
		stringMap _headers;
		std::string _body;
		std::string _statusCode;
		std::string _reasonPhrase;

	public:
		/* Constructor Method */
		Response( void );

		/* Destructor Method */
		virtual ~Response( void );

		/* Public Methods */
		std::string	generateResponse( void ) const;
		void	handleError( std::string _statusCode, 
			const std::string &error_page, const std::string &error_message);
		void	handleFileResponse( const std::string &path );
};

#endif // RESPONSE_HPP
