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
	std::string line;
	ServerConfigs serverConfig;
	LocationConfigs locationConfig;
	bool insideServer = false;
	bool insideLocation = false;

	while (std::getline(configFile, line)) {
		line = Utils::trim(line);
		if (line.empty() || line[0] == '#') continue;

		if (line.find("server {") != std::string::npos) {
			insideServer = true;
			serverConfig = ServerConfigs();
			continue;
		}

		if (insideServer && line.find("}") != std::string::npos) {
			insideServer = false;
			_servers.push_back(serverConfig);
			continue;
		}

		if (insideServer && line.find("location {") != std::string::npos) {
			insideLocation = true;
			locationConfig = LocationConfigs();
			continue;
		}

		if (insideLocation && line.find("}") != std::string::npos) {
			insideLocation = false;
			serverConfig.locations.push_back(locationConfig);
			continue;
		}

		if (insideServer && !insideLocation) {
            if (line.find("host") != std::string::npos) {
                serverConfig.host = Utils::extractValue(line, "host");
            } else if (line.find("listen") != std::string::npos) {
                serverConfig.port = std::stoi(Utils::extractValue(line, "listen"));
            } else if (line.find("server_name") != std::string::npos) {
                serverConfig.server_name = Utils::extractValue(line, "server_name");
            } else if (line.find("error_page") != std::string::npos) {
                int errorCode = Utils::extractErrorCode(line);
                std::string errorPage = Utils::extractErrorPage(line);
                serverConfig.error_pages[errorCode] = errorPage;
            } else if (line.find("limit_body_size") != std::string::npos) {
                serverConfig.limit_body_size = std::stoi(Utils::extractValue(line, "limit_body_size"));
            }
        }

        if (insideLocation) {
            if (line.find("methods") != std::string::npos) {
                locationConfig.methods = Utils::parseMethods(line);
            } else if (line.find("location_path") != std::string::npos) {
                locationConfig.location_path = Utils::extractValue(line, "location_path");
            } else if (line.find("root") != std::string::npos) {
                locationConfig.root = Utils::extractValue(line, "root");
            } else if (line.find("index") != std::string::npos) {
                locationConfig.index = Utils::extractValue(line, "index");
            } else if (line.find("autoindex") != std::string::npos) {
                locationConfig.autoindex = (Utils::extractValue(line, "autoindex") == "on");
            } else if (line.find("upload_enabled") != std::string::npos) {
                locationConfig.upload_enabled = (Utils::extractValue(line, "upload_enabled") == "on");
            } else if (line.find("upload_path") != std::string::npos) {
                locationConfig.upload_path = Utils::extractValue(line, "upload_path");
            } else if (line.find("cgi_enabled") != std::string::npos) {
                locationConfig.cgi_enabled = (Utils::extractValue(line, "cgi_enabled") == "on");
            } else if (line.find("cgi_extension") != std::string::npos) {
                locationConfig.cgi_extension = Utils::extractValue(line, "cgi_extension");
            } else if (line.find("cgi_path") != std::string::npos) {
                locationConfig.cgi_path = Utils::extractValue(line, "cgi_path");
            } else if (line.find("redirect") != std::string::npos) {
                locationConfig.redirect = Utils::extractValue(line, "redirect");
            }
        }
    }
}
