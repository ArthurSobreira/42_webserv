#ifndef CGI_HPP
#define CGI_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Request.hpp"

class CGI {
	private:
		int	_returnCode;
		std::string	_returnbody;
		Request		&_request;
		CGIConfigs	&_cgiConfig;
		std::map<std::string, std::string> _env;
		
	public:
		/* Constructor Method */
		CGI( Request &request, CGIConfigs &cgiConfig );

		/* Destructor Method */
		~CGI( void );
};

#endif
