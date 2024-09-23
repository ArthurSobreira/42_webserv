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

	public:
		/* Constructor Method */
		Config( const std::string &fileName,
			Logger &logger );

		/* Destructor Method */
		~Config( void );

		/* Public Methods */
		std::vector<ServerConfigs> getServers( void ) const;
		void _parseConfigFile( std::ifstream &configFile );
		void _parseServerBlock( const std::string &serverBlock, 
			int serverIndex );
		void _extractHost( std::vector<std::string> &tokens, 
			ServerConfigs &server );
		void _extractPort( std::vector<std::string> &tokens, 
			ServerConfigs &server );
		void _extractServerName( std::vector<std::string> &tokens, 
			ServerConfigs &server );
		void _extractLimitBodySize( std::vector<std::string> &tokens, 
			ServerConfigs &server );
		void _extractErrorPages( std::vector<std::string> &tokens,
			ServerConfigs &server );
};

namespace ConfigUtils {
	short	getServerCount( const std::string &fileName );
	void	validateLocationBrackets( const std::string &serverBlock );
	std::string	trimServerBlock( const std::string &serverBlock );
	std::string shortToString( const short &value );
	void	printServerStruct( const ServerConfigs &server );
}

#endif
