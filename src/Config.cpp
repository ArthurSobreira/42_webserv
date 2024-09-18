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
		short braceCount = 0;
		std::string line;
		while (std::getline(configFile, line)) {
			if (line.find("server_name") == std::string::npos && 
				line.find("server") != std::string::npos) {
				serverCount++;
			} 
			if (line.find("{") != std::string::npos || 
				line.find("}") != std::string::npos) {
				braceCount++;
			}
			if (line.find("{") != std::string::npos && 
				line.find("}") != std::string::npos) {
				throw std::runtime_error(ERROR_INVALID_SERVER);
			}
		}
		if (braceCount != serverCount * 2) {
			throw std::runtime_error(ERROR_INVALID_SERVER);
		}
		configFile.close();
		return (serverCount);
	}

	std::string	trimServerBlock( const std::string &serverBlock ) {
		std::stringstream	timmedServerBlock;
		std::istringstream	serverStream(serverBlock);
		std::string	line;

		while (std::getline(serverStream, line)) {
			size_t first = line.find_first_not_of(' ');
			if (first == std::string::npos) {
				continue;
			}
			size_t last = line.find_last_not_of(' ');
			line = line.substr(first, (last - first + 1));

			if (line.empty()) {
				continue;
			}

			if ((line.find("server") != std::string::npos &&
				line.find("server_name") == std::string::npos) ||
				line.find("{") != std::string::npos ||
				line.find("}") != std::string::npos) {
				continue;
			}

			size_t firstNonSpacePos = line.find_first_not_of(" \t");
			if (firstNonSpacePos != std::string::npos) {
				line = line.substr(firstNonSpacePos);
			}

			if (line.find("#") != std::string::npos) {
				if (line[0] == '#') {
					continue;
				} else {
					line = line.substr(0, line.find("#"));
				}
			}
			timmedServerBlock << line << std::string("\n");
		}
		return (timmedServerBlock.str());
	}

	std::string shortToString( const short &value ) {
		std::stringstream ss;
		ss << value;
		return (ss.str());
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
		_logger.logDebug(LOG_DEBUG, "Config file: " + fileName, true);
		_serverCount = ConfigUtils::getServerCount(fileName);
		_logger.logDebug(LOG_DEBUG, "Server count: " 
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
	return (this->_servers);
}

void Config::_parseConfigFile( std::ifstream &configFile ) {
	std::string line;
	std::string serverBlock;
	int braceCount = 0;
	int	serverIndex = 0;
	bool insideServerBlock = false;

	while (std::getline(configFile, line)) {
		if (line.find("server_name") == std::string::npos && 
			line.find("server") != std::string::npos) {
			size_t serverEndPos = line.find("server") + std::string("server").length();

			while (serverEndPos <= line.length() && std::isspace(line[serverEndPos])) {
				serverEndPos++;
			}

			if (serverEndPos <= line.length() && 
				(line[serverEndPos] == '{' || line[serverEndPos] == '\0')) {
				insideServerBlock = true;
			}
		}

		if (insideServerBlock) {
			serverBlock += line + std::string("\n");
			for (std::string::iterator it = line.begin(); 
				it != line.end(); ++it) {
				char chr = *it;
				if (chr == '{') {
					braceCount++;
				} else if (chr == '}') {
					braceCount--;

					if (braceCount < 0) {
						throw std::runtime_error(ERROR_EXTRA_CLOSE_BRACE);
					}
				}
			}

			if (braceCount == 0) {
				if (serverBlock.find("server") != std::string::npos &&
					serverBlock.find("{") == std::string::npos) {
					continue;
				}
				insideServerBlock = false;
				_parseServerBlock(serverBlock, serverIndex);
				serverBlock.clear();
				serverIndex++;
				braceCount = 0;
			}
		}
	}
	if (braceCount != 0) {
		throw std::runtime_error(ERROR_UNCLOSED_BLOCK);
	}
}

void	Config::_parseServerBlock( const std::string &serverBlock, int serverIndex ) {
	_logger.logDebug(LOG_DEBUG, "Server index: " 
		+ ConfigUtils::shortToString(serverIndex), true);
	_logger.logDebug(LOG_DEBUG, "Server block: \n" + serverBlock, true);

	std::string	trimmedBlock = ConfigUtils::trimServerBlock(serverBlock);
	_logger.logDebug(LOG_DEBUG, "Trimmed block: \n" + trimmedBlock, true);
	// std::istringstream serverStream(trimmedBlock);
	// ServerConfigs server;
	// std::string line;
	// while (std::getline(serverStream, line)) {
	// 	if (line.find("server_name") != std::string::npos) {
	// 		server.server_name = line.substr(line.find("server_name") + std::string("server_name").length());
	// 		server.server_name.erase(std::remove(server.server_name.begin(), server.server_name.end(), ' '), server.server_name.end());
	// 	}
	// 	if (line.find("host") != std::string::npos) {
	// 		server.host = line.substr(line.find("host") + std::string("host").length());
	// 		server.host.erase(std::remove(server.host.begin(), server.host.end(), ' '), server.host.end());
	// 	}
	// 	if (line.find("port") != std::string::npos) {
	// 		server.port = std::stoi(line.substr(line.find("port") + std::string("port").length()));
	// 	}
	// 	if (line.find("error_page") != std::string::npos) {
	// 		std::string errorPage = line.substr(line.find("error_page") + std::string("error_page").length());
	// 		errorPage.erase(std::remove(errorPage.begin(), errorPage.end(), ' '), errorPage.end());
	// 		std::string errorCode = errorPage.substr(0, errorPage.find(" "));
	// 		std::string errorPath = errorPage.substr(errorPage.find(" ") + 1);
	// 		server.error_pages[std::stoi(errorCode)] = errorPath;
	// 	}
	// 	if (line.find("limit_body_size") != std::string::npos) {
	// 		server.limit_body_size = std::stoi(line.substr(line.find("limit_body_size") + std::string("limit_body_size").length()));
	// 	}
	// }
}
