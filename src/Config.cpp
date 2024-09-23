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

void	Config::_extractHost( std::vector<std::string> &tokens, 
	ServerConfigs &server ) {
	if (tokens.size() < 2 || tokens[1].empty()) {
		throw std::runtime_error(ERROR_MISSING_VALUE);
	}
	uint32_t temp_ip;
	if (!inetPton(tokens[1], temp_ip, _logger)) {
		throw std::runtime_error(ERROR_INVALID_HOST);
	}
	server.host = tokens[1];
}

void	Config::_extractPort( std::vector<std::string> &tokens, 
	ServerConfigs &server ) {
	if (tokens.size() < 2 || tokens[1].empty()) {
		throw std::runtime_error(ERROR_MISSING_VALUE);
	}
	std::stringstream stringPort(tokens[1]);
	if (stringPort >> server.port) {
		if (server.port < 1024 || server.port > 65535) {
			throw std::runtime_error(ERROR_INVALID_PORT);
		}
	} else {
		throw std::runtime_error(ERROR_INVALID_PORT);
	}
}

void	Config::_extractServerName( std::vector<std::string> &tokens, 
	ServerConfigs &server ) {
	if (tokens.size() < 2 || tokens[1].empty()) {
		throw std::runtime_error(ERROR_MISSING_VALUE);
	}
	server.serverName = tokens[1];
}

void	Config::_extractLimitBodySize( std::vector<std::string> &tokens, 
	ServerConfigs &server ) {
	if (tokens.size() < 2 || tokens[1].empty()) {
		throw std::runtime_error(ERROR_MISSING_VALUE);
	}
	std::stringstream stringSize(tokens[1]);
	long long limitBodySize;
	if (stringSize >> limitBodySize) {
		if (limitBodySize < 0) {
			throw std::runtime_error(ERROR_INVALID_LIMIT_BODY_SIZE);
		}
		server.limitBodySize = static_cast<size_t>(limitBodySize);
	} else {
		throw std::runtime_error(ERROR_INVALID_LIMIT_BODY_SIZE);
	}
}

static void	printServerStruct( const ServerConfigs &server );

void	Config::_parseServerBlock( const std::string &serverBlock, int serverIndex ) {
	std::string	trimmedBlock = ConfigUtils::trimServerBlock(serverBlock);
	std::istringstream serverStream(trimmedBlock);
	ServerConfigs server;
	std::string line;

	_logger.logDebug(LOG_DEBUG, "Server index: " 
			+ ConfigUtils::shortToString(serverIndex), true);
	_logger.logDebug(LOG_DEBUG, "Trimmed block: \n" + trimmedBlock, true);

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

		// else if (!tokens.empty() && tokens[0] == "limit_body_size") {
		// 	if (tokens.size() < 2 || tokens[1].empty()) {
		// 		throw std::runtime_error(ERROR_MISSING_VALUE);
		// 	}
		// 	std::stringstream stringSize(tokens[1]);
		// 	if (stringSize >> server.limitBodySize) {
		// 		if (server.limitBodySize < 0) {
		// 			throw std::runtime_error(ERROR_INVALID_PORT);
		// 		}
		// 	} else {
		// 		throw std::runtime_error(ERROR_INVALID_PORT);
		// 	}
		// }

		// else if (!tokens.empty() && tokens[0] == "error_page") {
		// 	if (tokens.size() < 3 || tokens[1].empty() || tokens[2].empty()) {
		// 		throw std::runtime_error(ERROR_MISSING_VALUE);
		// 	}
		// 	server.errorPages[tokens[1]] = tokens[2];
		// }

		// else if (!tokens.empty() && tokens[0] == "location") {
		// 	LocationConfigs location;
		// 	std::string locationBlock;
		// 	int locationBraceCount = 0;
		// 	bool insideLocationBlock = false;

		// 	while (std::getline(serverStream, line)) {
		// 		if (line.find("location_path") == std::string::npos && 
		// 			line.find("location") != std::string::npos) {
		// 			size_t locationEndPos = line.find("location") + std::string("location").length();

		// 			while (locationEndPos <= line.length() && std::isspace(line[locationEndPos])) {
		// 				locationEndPos++;
		// 			}

		// 			if (locationEndPos <= line.length() && 
		// 				(line[locationEndPos] == '{' || line[locationEndPos] == '\0')) {
		// 				insideLocationBlock = true;
		// 			}
		// 		}

		// 		if (insideLocationBlock) {
		// 			locationBlock += line + std::string("\n");
		// 			for (std::string::iterator it = line.begin(); 
		// 				it != line.end(); ++it) {
		// 				char chr = *it;
		// 				if (chr == '{') {
		// 					locationBraceCount

	}
	_servers.push_back(server);
	printServerStruct(getServers()[serverIndex]);
}

static void	printServerStruct( const ServerConfigs &server ) {
	std::cout << "              Server Configs " << std::endl;
	std::cout << "=========================================" << std::endl;
	std::cout << "port: " << server.port << std::endl;
	std::cout << "host: " << server.host << std::endl;
	std::cout << "serverName: " << server.serverName << std::endl;
	std::cout << "limitBodySize: " << server.limitBodySize << std::endl;
	for (errorMap::const_iterator it = server.errorPages.begin(); 
		it != server.errorPages.end(); ++it) {
		std::cout << "errorPages: " << it->first << " " << it->second << std::endl;
	}
	std::cout << "[ Server Locations ]: " << std::endl;
	for (std::vector<LocationConfigs>::const_iterator it = server.locations.begin(); 
		it != server.locations.end(); ++it) {
		std::cout << "\tHTTP Method: ";
		for (std::vector<httpMethod>::const_iterator it2 = it->methods.begin(); 
			it2 != it->methods.end(); ++it2) {
			if (*it2 == GET) {
				std::cout << "GET";
			} else if (*it2 == POST) {
				std::cout << "POST";
			} else if (*it2 == DELETE) {
				std::cout << "DELETE";
			}
			std::cout << " ";
		}
		std::cout << std::endl;
		std::cout << "\tlocationPath: " << it->locationPath << std::endl;
		std::cout << "\troot: " << it->root << std::endl;
		std::cout << "\tindex: " << it->index << std::endl;
		std::cout << "\tredirect: " << it->redirect << std::endl;
		std::cout << "\tuploadPath: " << it->uploadPath << std::endl;
		std::cout << "\tautoindex: " << it->autoindex << std::endl;
		std::cout << "\tuploadEnabled: " << it->uploadEnabled << std::endl;
		std::cout << "\tCGI path: " << it->cgiConfig.cgiPath << std::endl;
		std::cout << "\tCGI extension: " << it->cgiConfig.cgiExtension << std::endl;
		std::cout << "\tCGI enabled: " << it->cgiConfig.cgiEnabled << std::endl;
	}
}
