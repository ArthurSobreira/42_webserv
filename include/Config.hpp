#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "includes.hpp"

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
	std::string	host;
	const int	port;
	std::string server_name;
	errorMap error_pages;
	const int limit_body_size;
	std::vector<LocationConfigs> locations;
};

class Config {
	private:
		const std::string _fileName;
		std::vector<ServerConfigs> _servers;

	public:
		/* Constructor Methods */
		Config( void );
		Config( const std::string &fileName );
		
		/* Destructor Method */
		~Config( void );
		
		/* Public Methods */
		void parseConfigFile( void );
};

#endif
