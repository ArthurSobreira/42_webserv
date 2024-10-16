#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"

/* Struct LocationConfigs Constructor */
LocationConfigs::LocationConfigs( void ) {
	methods.push_back(GET);
	locationPath = DEFAULT_LOCATION_PATH;
	root = DEFAULT_ROOT;
	index = DEFAULT_INDEX;
	redirect = DEFAULT_EMPTY;
	uploadPath = DEFAULT_UPLOAD_PATH;
	autoindex = false;
	uploadEnabled = false;
	rootSet = false;
	redirectSet = false;
	cgiEnabled = false;
	cgiPath = DEFAULT_EMPTY;
	cgiExtension = DEFAULT_EMPTY;
}

/* Struct ServerConfigs Constructor */
ServerConfigs::ServerConfigs( void ) {
	port = DEFAULT_PORT;
	host = DEFAULT_HOST;
	serverName = DEFAULT_SERVER_NAME;
	limitBodySize = DEFAULT_LIMIT_BODY_SIZE;
	errorPages["400"] = DEFAULT_ERROR_400;
	errorPages["403"] = DEFAULT_ERROR_403;
	errorPages["404"] = DEFAULT_ERROR_404;
	errorPages["405"] = DEFAULT_ERROR_405;
	errorPages["500"] = DEFAULT_ERROR_500;
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
		_serverCount = ConfigUtils::getServerCount(fileName);
		_logger.logDebug(LOG_DEBUG, "Starting to parse " + fileName, true);
		_parseConfigFile(configFile);
		configFile.close();
		return ;
	}
};

/* Destructor Method */
Config::~Config( void ) {};

/* Private Methods */
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
				braceCount = 0;
				insideServerBlock = false;
				_parseServerBlock(serverBlock);
				serverBlock.clear();
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
	std::set<std::string> serverKeys;
	ServerConfigs server;
	std::string line;

	while (std::getline(serverStream, line)) {
		if ((line[line.size() - 1] != '[' && line[line.size() - 1] != ']') &&
			(line.empty() || line[line.size() - 1] != ';')) {
			throw std::runtime_error(ERROR_INVALID_LINE);
		}
		std::istringstream tokenStream(line);
		std::string token;
		stringVector tokens;

		while (tokenStream >> token) {
			if (!token.empty() && token[token.size() - 1] == ';') {
				token = token.substr(0, token.size() - 1);
			}
			token.erase(std::remove_if(token.begin(), token.end(), 
				::isspace), token.end());

			if (token.empty()) { 
				continue; 
			} else { tokens.push_back(token); }
		}

		if (tokens.empty()) { continue; }

		if (tokens[0] != "error_page" && tokens[0] != "location" &&
			serverKeys.find(tokens[0]) != serverKeys.end()) {
			throw std::runtime_error(ERROR_DUPLICATE_KEY);
		} else { serverKeys.insert(tokens[0]); }

		if (tokens[0] == "listen") { ServerExtraction::port(tokens, server); }
		else if (tokens[0] == "host") { ServerExtraction::host(tokens, server); }
		else if (tokens[0] == "server_name") { ServerExtraction::serverName(tokens, server); }
		else if (tokens[0] == "limit_body_size") { ServerExtraction::limitBodySize(tokens, server); }
		else if (tokens[0] == "error_page") { ServerExtraction::errorPages(tokens, server); }
		else if (tokens[0] == "location") {
			if (tokens[1].empty() || tokens.size() != 2 || tokens[1] != "[") { 
				throw std::runtime_error(ERROR_INVALID_LOCATION);
			} else { _parseLocationStream(serverStream, server); }
		} else { throw std::runtime_error(ERROR_INVALID_KEY); }
	}
	_servers.push_back(server);
	ConfigUtils::printServerStruct(server);
}

void	Config::_parseLocationStream( std::istringstream &serverStream, ServerConfigs &server ) {
	std::string line;
	std::string locationBlock;
	LocationConfigs location;
	int locationBracketsCount = 1;
	bool insideLocationBlock = true;

	while (std::getline(serverStream, line)) {
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
				locationBracketsCount = 0;
				insideLocationBlock = false;
				_parseLocationBlock(locationBlock, location);
				server.locations.push_back(location);
				locationBlock.clear();
				return ;
			}
		}
	}
	if (locationBracketsCount != 0) {
		throw std::runtime_error(ERROR_INVALID_LOCATION);
	}
}

void	Config::_parseLocationBlock( const std::string &locationBlock, LocationConfigs &location ) {
	std::istringstream locationStream(locationBlock);
	std::set<std::string> locationKeys;
	std::string line;

	while (std::getline(locationStream, line)) {
		if (line.empty() || line[line.size() - 1] != ';') {
			throw std::runtime_error(ERROR_INVALID_LINE);
		}
		std::istringstream tokenStream(line);
		std::string token;
		stringVector tokens;

		while (tokenStream >> token) {
			if (!token.empty() && token[token.size() - 1] == ';') {
				token = token.substr(0, token.size() - 1);
			}
			tokens.push_back(token);
		}

		if (tokens.empty()) { continue; }

		if (locationKeys.find(tokens[0]) != locationKeys.end()) {
			throw std::runtime_error(ERROR_DUPLICATE_KEY);
		} else { locationKeys.insert(tokens[0]); }

		if (tokens[0] == "methods") { LocationExtraction::methods(tokens, location); }
		else if (tokens[0] == "location_path") { LocationExtraction::locationPath(tokens, location); }
		else if (tokens[0] == "root") { LocationExtraction::root(tokens, location); }
		else if (tokens[0] == "index") { LocationExtraction::index(tokens, location); }
		else if (tokens[0] == "redirect") { LocationExtraction::redirect(tokens, location); }
		else if (tokens[0] == "upload_path") { LocationExtraction::uploadPath(tokens, location); }
		else if (tokens[0] == "autoindex") { LocationExtraction::autoindex(tokens, location); }
		else if (tokens[0] == "upload_enabled") { LocationExtraction::uploadEnabled(tokens, location); }
		else if (tokens[0] == "cgi_path") { LocationExtraction::cgiPath(tokens, location); }
		else if (tokens[0] == "cgi_extension") { LocationExtraction::cgiExtension(tokens, location); }
		else { throw std::runtime_error(ERROR_INVALID_KEY); }
	}
	ConfigUtils::validateFullLocationPath(location);
	ConfigUtils::validateFullCGIPath(location);
	ConfigUtils::createUploadFolder(location.uploadPath);
}

/* Public Methods */
std::vector<ServerConfigs> Config::getServers( void ) const {
	return (this->_servers);
}

std::map<int,const ServerConfigs*> Config::getSocketConfigMap( void ) const {
	return (this->_socketConfigMap);
}

void Config::setSocketConfigMap( const int &socket, const ServerConfigs *config ) {
	this->_socketConfigMap[socket] = config;
}

const ServerConfigs *Config::getServerConfig( const int &socket ) {
	return (this->_socketConfigMap[socket]);
}

void Config::setSocketServerMap( const int &socket, const int &server ){
	this->_socketServerMap[socket] = server;
}

int Config::getServerSocket( const int &socket ){
	return (this->_socketServerMap[socket]);
}

const LocationConfigs Config::getLocationConfig( const ServerConfigs &serverConfig,
	const std::string &uri, bool &locationFound ) const {
	LocationConfigs bestMatch;
	size_t bestMatchLength = 0;

	for (std::vector<LocationConfigs>::const_iterator it = serverConfig.locations.begin(); 
		it != serverConfig.locations.end(); ++it) {
		if (uri.find(it->locationPath) == 0 && it->locationPath.length() > bestMatchLength) {
			bestMatch = *it;
			locationFound = true;
			bestMatchLength = it->locationPath.length();
		}
	}
	return (bestMatch);
}
