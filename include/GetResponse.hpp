#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Response.hpp"

class GetResponse : public Response {
	private:
		std::string _filePath;
		stringMap _headers;

	public:
		/* Constructor Method */
		GetResponse( std::string filePath );

		/* Destructor Method */
		~GetResponse( void );

		/* Public Methods */
		void listDirectoryHandler( void );
		void prepareResponse( const LocationConfigs &location );
};

#endif // GETRESPONSE_HPP
