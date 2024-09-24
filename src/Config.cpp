#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"

/* Struct CGIConfigs Constructor */
CGIConfigs::CGIConfigs( void ) {
	cgiPath = DEFAULT_CGI_PATH;
	cgiExtension = DEFAULT_CGI_EXT;
	cgiEnabled = false;
}

/* Struct LocationConfigs Constructor */
LocationConfigs::LocationConfigs( void ) {
	methods.push_back(GET);
	locationPath = DEFAULT_LOCATION_PATH;
	root = DEFAULT_ROOT;
	index = DEFAULT_INDEX;
	redirect = DEFAULT_REDIRECT;
	uploadPath = DEFAULT_UPLOAD_PATH;
	autoindex = false;
	uploadEnabled = false;
	cgiConfig = CGIConfigs();
}

/* Struct ServerConfigs Constructor */
ServerConfigs::ServerConfigs( void ) {
	port = DEFAULT_PORT;
	host = DEFAULT_HOST;
	serverName = DEFAULT_SERVER_NAME;
	limitBodySize = DEFAULT_LIMIT_BODY_SIZE;
	errorPages["403"] = DEFAULT_ERROR_403;
	errorPages["404"] = DEFAULT_ERROR_404;
	locations.push_back(LocationConfigs());
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
		_parseConfigFile(configFile);
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
						throw std::runtime_error(ERROR_INVALID_SERVER);
					}
				}
			}
			if (braceCount == 0) {
				if (serverBlock.find("server") != std::string::npos &&
					serverBlock.find("{") == std::string::npos) {
					continue;
				}
				insideServerBlock = false;
				_parseServerBlock(serverBlock);
				serverBlock.clear();
				braceCount = 0;
			}
		}
	}
	if (braceCount != 0) {
		throw std::runtime_error(ERROR_INVALID_SERVER);
	}
}

void	Config::_parseServerBlock( const std::string &serverBlock ) {
	std::string	trimmedBlock = ConfigUtils::trimServerBlock(serverBlock);
	std::istringstream serverStream(trimmedBlock);
	ServerConfigs server;
	std::string line;

	ConfigUtils::validateLocationBrackets(trimmedBlock);
	while (std::getline(serverStream, line)) {
		if ((line[line.size() - 1] != '[' && line[line.size() - 1] != ']') &&
			(line.empty() || line[line.size() - 1] != ';')) {
			throw std::runtime_error(ERROR_INVALID_LINE);
		}
		std::istringstream tokenStream(line);
		std::string token;
		std::vector<std::string> tokens;

		while (tokenStream >> token) {
			if (!token.empty() && token[token.size() - 1] == ';') {
				token = token.substr(0, token.size() - 1);
			}
			tokens.push_back(token);
		}

		if (tokens.empty()) {
			continue;
		}

		if (tokens[0] == "host") { _extractHost(tokens, server); }
		else if (tokens[0] == "listen") { _extractPort(tokens, server); }
		else if (tokens[0] == "server_name") { _extractServerName(tokens, server); }
		else if (tokens[0] == "limit_body_size") { _extractLimitBodySize(tokens, server); }
		else if (tokens[0] == "error_page") { _extractErrorPages(tokens, server); }
		else if (tokens[0] == "location") {
			if (tokens[1].empty() || tokens.size() < 2 || tokens[1] != "[") { 
				throw std::runtime_error(ERROR_INVALID_LOCATION);
			} else { _parseLocationStream(serverStream, server); }
		} else { throw std::runtime_error(ERROR_INVALID_KEY); }
	}
	_servers.push_back(server);
	ConfigUtils::printServerStruct(getServers()[0]);
}

void	Config::_parseLocationStream( std::istringstream &serverStream, ServerConfigs &server ) {
	std::string locationBlock;
	int locationBracketsCount = 1;
	bool insideLocationBlock = true;
	LocationConfigs location;
	std::string line;

	while (std::getline(serverStream, line)) {  // Por alguma razão isso reseta o stream
		if (insideLocationBlock) {
			if (line.find("]") == std::string::npos) {
				locationBlock += line + std::string("\n");
			}
			for (std::string::iterator it = line.begin(); 
				it != line.end(); ++it) {
				char chr = *it;
				if (chr == ']') {
					locationBracketsCount--;

					if (locationBracketsCount < 0) {
						throw std::runtime_error(ERROR_INVALID_LOCATION);
					}
				}
			}

			if (locationBracketsCount == 0) {
				insideLocationBlock = false;
				// locationBlock = ConfigUtils::trimServerBlock(locationBlock);
				_logger.logDebug(LOG_DEBUG, "Complete Location block: \n" + locationBlock, true);
				// location = ConfigUtils::parseLocationBlock(locationBlock);
				// server.locations.push_back(location);
				(void)server;
				locationBlock.clear();
				locationBracketsCount = 0;
			}
		}
	}
	if (locationBracketsCount != 0) {
		throw std::runtime_error(ERROR_INVALID_LOCATION);
	}
}
