#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"

namespace ConfigUtils {
	short	getServerCount( const std::string &fileName ) {
		std::ifstream configFile(fileName.c_str());
		if (!configFile.is_open() || configFile.fail()) {
			throw std::runtime_error(ERROR_OPEN_CONFIG_FILE);
		}

		short serverCount = 0;
		std::string line;
		while (std::getline(configFile, line)) {
			if (line.find("server_name") == std::string::npos && 
				line.find("server") != std::string::npos) {
				serverCount++;
			}
		}
		configFile.close();
		return (serverCount);
	}

	std::string shortToString( const short &value ) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
}

/* Constructor Method */
Config::Config( const std::string &fileName, Logger &logger ) 
	: _fileName(fileName), _logger(logger) {
	std::ifstream configFile(fileName.c_str());

	if (!configFile.is_open() || configFile.fail()) {
		throw std::runtime_error(ERROR_OPEN_CONFIG_FILE);
	}

	if (configFile.peek() == std::ifstream::traits_type::eof()) {
		throw std::runtime_error(ERROR_EMPTY_CONFIG_FILE);
	}

	if (configFile.is_open()) {
		_logger.logDebug("Config file: " + fileName, true);
		_serverCount = ConfigUtils::getServerCount(fileName);
		_logger.logDebug("Server count: " 
				+ ConfigUtils::shortToString(_serverCount), true);
		this->_parseConfigFile(configFile);
		configFile.close();
		return ;
	}
};

/* Destructor Method */
Config::~Config( void ) {};

/* Public Methods */
std::vector<ServerConfigs> Config::getServers( void ) const {
	return this->_servers;
}

void Config::_parseConfigFile( std::ifstream &configFile ) {
	// static short serverCount = 0;
	// static short locationCount = 0;

	std::string line;
	while (std::getline(configFile, line)) {
		_logger.logDebug(line);
	}
}
