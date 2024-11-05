#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class CGIResponse : public Response {
	private:
		std::string	_cgiPath;
		std::string	_cgiExecutable;
		Request		_request;
		LocationConfigs	_locationConfig;
		Logger	_logger;
		std::map<std::string, std::string> _env;
		
		/* Private Methods */
		void	_setEnvironmentVars( void );
		char	**_generateEnvp( void );
		std::string	_getContentLength( void ) const;
		std::string	_getExecutable( const std::string &extension );
		std::string	_getQueryString( const std::string &uri ) const;
		std::string	_getPathInfo( const std::string &uri ) const;
		void	_handleCGIError( int code, const std::string &message );
		bool	_waitChild( pid_t pid, int &status, std::clock_t start );
		void	_readReturnBody( int pipefd[2] );

	public:
		/* Constructor Method */
		CGIResponse( const Request &request, 
			const LocationConfigs &location );

		/* Destructor Method */
		~CGIResponse( void );

		/* Public Methods */
		void	executeCGI( void );
};

namespace CGIUtils {
	bool	methodIsOnLocation( LocationConfigs &location, 
		httpMethod method );
	bool	isUploadRequest(const Request &request);
	std::string extractFileName( const std::string &body );
	std::string	intToString( int value );
	void	deleteEnvp( char **envp );
}

#endif