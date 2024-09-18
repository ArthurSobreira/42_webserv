#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Utils.hpp"
#include "Includes.hpp"
#include "Logger.hpp"

typedef enum {
	GET,
	POST,
	DELETE,
} httpMethod;

struct LocationConfigs {
	std::vector<httpMethod> methods;
	std::string location_path;
	std::string root;
	std::string index;
	std::string redirect;
	std::string upload_path;
	std::string cgi_path;
	std::string cgi_extension;
	bool autoindex;
	bool upload_enabled;
	bool cgi_enabled;
};

struct ServerConfigs {
	unsigned short	port;
	std::string	host;
	std::string server_name;
	size_t limit_body_size;
	errorMap error_pages;
	std::vector<LocationConfigs> locations;
};

class Config {
	private:
		const std::string _fileName;
		short _serverCount;
		std::vector<ServerConfigs> _servers;
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
};

#endif
