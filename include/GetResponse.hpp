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
		void listDirectoryHandler( const LocationConfigs &location );
		void prepareResponse( const LocationConfigs &location );
	private:
		void addHeader(const std::string &title);
		bool listDirectory(const std::string &dirPath);
		void addFileEntry(std::vector<std::string> &folders, std::vector<std::string> &files, std::map<std::string, status> &filesDetails);
		void addFooter();
};

#endif // GETRESPONSE_HPP
