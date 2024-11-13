#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Response.hpp"

class PostResponse : public Response {
	private:
		std::string	_filePath;
		std::string	_postData;
		std::string	_responseData;
		stringMap	_headersRequest;
		std::string	_fileName;
		LocationConfigs	_location;
		std::set<std::string>	_validTypes;
		std::string	_contentType;

	public:
		/* Constructor Method */
		PostResponse( std::string filePath, std::string postData, 
			stringMap headersRequest, LocationConfigs location );
		
		/* Destructor Method */
		~PostResponse( void );

		/* Public Method */
		void	prepareResponse( void );

	private:
		int		_createFile( void );
		bool	_isValidContentTypeAndSetExtension( void );
		void	_removeCarriageReturn( void );
		void	_removeBoundary( void );
};

#endif // POSTRESPONSE_HPP
