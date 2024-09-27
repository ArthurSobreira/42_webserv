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
	bool rootSet;
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
		void _parseLocationStream( std::istringstream &serverStream, 
			ServerConfigs &server );
		void _parseLocationBlock( const std::string &locationBlock, 
			LocationConfigs &location );

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
	bool	hostIsValid( ServerConfigs &server );
	bool	isRepeatedMethod( std::vector<httpMethod> &methodsVector, 
		httpMethod method );
	bool	directoryExists( const std::string &path );
	void	formatPath( std::string &path );
	void	validateFullLocationPath( LocationConfigs &location );
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

/* Location Extraction Functions */
namespace LocationExtraction {
	void	methods( stringVector &tokens, LocationConfigs &location );
	void	locationPath( stringVector &tokens, LocationConfigs &location );
	void	root( stringVector &tokens, LocationConfigs &location );
	void	index( stringVector &tokens, LocationConfigs &location );
	void	redirect( stringVector &tokens, LocationConfigs &location );
	void	uploadPath( stringVector &tokens, LocationConfigs &location );
	void	autoindex( stringVector &tokens, LocationConfigs &location );
	void	uploadEnabled( stringVector &tokens, LocationConfigs &location );
}

#endif
