#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"

/* Constructor Method */
Config::Config( const std::string &fileName, Logger &logger ) 
	: _fileName(fileName), _logger(logger) {
	std::ifstream configFile(fileName.c_str());

	if (!configFile.is_open() || configFile.fail()) {
		throw std::runtime_error(ERROR_OPEN_CONFIG_FILE);
	}

	_logger.logDebug("Config file: " + fileName, true);
	if (configFile.is_open()) {
		this->_parseConfigFile(configFile);
		configFile.close();
		return ;
	}
};

/* Destructor Method */
Config::~Config( void ) {};

/* Public Methods */
void Config::_parseConfigFile( std::ifstream &configFile ) {
	std::string line;
	while (std::getline(configFile, line)) {
		_logger.logDebug(line);
	}
}
