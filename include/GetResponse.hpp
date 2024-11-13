#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Response.hpp"

class GetResponse : public Response {
	private:
		std::string _filePath;
		std::string _uri;
		stringMap _headers;

	public:
		/* Constructor Method */
		GetResponse( std::string filePath );

		/* Destructor Method */
		~GetResponse( void );

		/* Public Methods */
		void	prepareResponse( const LocationConfigs &location );
		void	listDirectoryHandler( const LocationConfigs &location );
	
	private:
		void	_addHeader( const std::string &title );
		bool	_listDirectory( const std::string &dirPath );
		void	_addFileEntry( std::vector<std::string> &folders, 
			std::vector<std::string> &files, std::map<std::string, status> &filesDetails );
		void	_addFooter( void );
};

#endif // GETRESPONSE_HPP
