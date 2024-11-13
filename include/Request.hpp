#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"

class Request {
	private:
		httpMethod	_method;
		std::string	_uri;
		std::string	_version;
		std::string	_body;
		std::string	_rawRequest;
		stringMap	_headers;
		LocationConfigs _location;
		bool _isCGI;
		bool _isRedirect;
		bool _connectionClose;
		std::string	_boundary;
		std::string	_queryString;

	public:
		/* Constructor Method */
		Request( const std::string &rawRequest, 
			bool completeRequest );

		/* Destructor Method */
		~Request( void );

		/* Getter Methods */
		httpMethod	getMethod( void ) const { return _method; }
		const std::string	&getUri( void ) const { return _uri; }
		const std::string	&getVersion( void ) const { return _version; }
		const std::string	&getBody( void ) const { return _body; }
		const stringMap		&getHeaders( void ) const { return _headers; }
		LocationConfigs		getLocation( void ) const { return _location; }
		bool isCGI( void ) const { return _isCGI; }
		bool isRedirect( void ) const { return _isRedirect; } 
		bool connectionClose( void ) const { return _connectionClose; }
		std::string	getQueryString( void ) const { return _queryString; }
		const std::string &getHeader( const std::string &name ) const;

		/* Public Method */
		std::string validateRequest( Config _config, ServerConfigs server, 
			bool completeRequest );
	
	private:
		void	_parseRequest( void );
		void	_parseMethodAndUri ( const std::string &line );
		void	_parserQueryString( void );
		void	_parseHeaders( const std::vector<std::string> &headerLines );
		void	_parseBody( void );
		void	_extractMultipartNamesAndFilenames( void );
		std::string	_folderPath( const std::string &uri );
		bool	_validateHost( ServerConfigs server );
		void	_checkConnectionClose( void );
		httpMethod	_parseMethod( const std::string &method );
};

#endif // REQUEST_HPP
