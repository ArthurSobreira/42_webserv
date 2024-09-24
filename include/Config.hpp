#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Utils.hpp"
#include "Includes.hpp"
#include "Logger.hpp"

typedef enum {
	GET = 1,
	POST,
	DELETE,
} httpMethod;

struct CGIConfigs {
	std::string cgiPath;
	std::string cgiExtension;
	bool cgiEnabled;

	/* Struct Constructor */
	CGIConfigs( void );
};

struct LocationConfigs {
	std::vector<httpMethod> methods;
	std::string locationPath;
	std::string root;
	std::string index;
	std::string redirect;
	std::string uploadPath;
	bool autoindex;
	bool uploadEnabled;
	CGIConfigs cgiConfig;

	/* Struct Constructor */
	LocationConfigs( void );
};

struct ServerConfigs {
	unsigned short	port;
	std::string	host;
	std::string serverName;
	size_t limitBodySize;
	errorMap errorPages;
	std::vector<LocationConfigs> locations;

	/* Struct Constructor */
	ServerConfigs( void );
};

class Config {
	private:
		const std::string _fileName;
		std::vector<ServerConfigs> _servers;
		short _serverCount;
		Logger &_logger;

		/* Private Methods */
		void _parseConfigFile( std::ifstream &configFile );
		void _parseServerBlock( const std::string &serverBlock );
		void _parseLocationStream( std::istringstream &serverStream, ServerConfigs &server );
		void _parseLocationBlock( const std::string &locationBlock, LocationConfigs &location );

		/* Location Extraction Methods */
		void _extractMethods( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractLocationPath( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractRoot( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractIndex( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractRedirect( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractUploadPath( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractAutoindex( std::vector<std::string> &tokens, LocationConfigs &location );
		void _extractUploadEnabled( std::vector<std::string> &tokens, LocationConfigs &location );

	public:
		/* Constructor Method */
		Config( const std::string &fileName,
			Logger &logger );

		/* Destructor Method */
		~Config( void );

		/* Public Methods */
		std::vector<ServerConfigs> getServers( void ) const;
};

/* Config Utils Functions */
namespace ConfigUtils {
	short	getServerCount( const std::string &fileName );
	std::string	trimServerBlock( const std::string &serverBlock );
	std::string	shortToString( const short &value );
	void	printServerStruct( const ServerConfigs &server );
}

/* Server Extraction Functions */
namespace ServerExtraction {
	void	port( stringVector &tokens, ServerConfigs &server );
	void	host( stringVector &tokens, ServerConfigs &server );
	void	serverName( stringVector &tokens, ServerConfigs &server );
	void	limitBodySize( stringVector &tokens, ServerConfigs &server );
	void	errorPages( stringVector &tokens, ServerConfigs &server );
}

#endif
