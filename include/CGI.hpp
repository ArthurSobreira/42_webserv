#ifndef CGI_HPP
#define CGI_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Request.hpp"

class CGI {
	private:
		int		_returnCode;
		std::string	_returnbody;
		std::string	_cgiPath;
		std::string _cgiExecutable;
		Request		_request;
		CGIConfigs	_cgiConfig;
		std::map<std::string, std::string> _env;
		
		/* Private Methods */
		void	_setEnvironmentVars( void );
		std::string _getQueryString( const std::string &uri ) const;
		std::string _getPathInfo( const std::string &uri ) const;

	public:
		/* Constructor Method */
		CGI( const Request &request, 
			const LocationConfigs &location );

		/* Destructor Method */
		~CGI( void );

		/* Public Methods */
		std::string	getCGIOutput( void );
		int	getReturnCode( void ) const;
		std::string	getReturnBody( void ) const;
};

#endif
