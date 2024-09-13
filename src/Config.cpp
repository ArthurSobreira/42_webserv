#include "Config.hpp"

/* Constructor Methods */
Config::Config( void ) { throw std::runtime_error("Error: File Not Passed"); };

Config::Config( const std::string &fileName ) : _fileName(fileName) {
	std::cout << "Config file: " << _fileName << std::endl;
};

/* Destructor Method */
Config::~Config( void ) {};

/* Public Methods */
void Config::parseConfigFile( void ) {
	return ;
}
