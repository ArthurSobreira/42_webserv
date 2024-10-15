#ifndef CGI_HPP
#define CGI_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class CGI {
	private:
		int		_returnCode;
		std::string	_returnbody;
		std::string	_cgiPath;
		std::string _cgiExecutable;
		Request		_request;
		ServerConfigs	_serverConfig;
		LocationConfigs	_locationConfig;
		Logger		_logger;
		std::map<std::string, std::string> _env;
		
		/* Private Methods */
		void	_setEnvironmentVars( void );
		std::string	_getContentLength( void ) const;
		std::string _getQueryString( const std::string &uri ) const;
		std::string _getPathInfo( const std::string &uri ) const;
		void	_handleCGIError( int code, const std::string &message );
		char	**_generateEnvp( void );

	public:
		/* Constructor Method */
		CGI( const Request &request, const ServerConfigs &server,
			const LocationConfigs &location );

		/* Destructor Method */
		~CGI( void );

		/* Public Methods */
		int	getReturnCode( void ) const;
		std::string	getReturnBody( void ) const;
		void	executeCGI( void );
};

#endif
