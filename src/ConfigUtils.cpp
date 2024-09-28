#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Server.hpp"

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

	std::string	shortToString( const short &value ) {
		std::stringstream ss;
		ss << value;
		return (ss.str());
	}

	void	printServerStruct( const ServerConfigs &server ) {
		std::cout << "              Server Configs " << std::endl;
		std::cout << "=========================================" << std::endl;
		std::cout << "port: " << server.port << std::endl;
		std::cout << "host: " << server.host << std::endl;
		std::cout << "server_name: " << server.serverName << std::endl;
		std::cout << "limit_body_size: " << server.limitBodySize << std::endl;
		for (errorMap::const_iterator it = server.errorPages.begin(); 
			it != server.errorPages.end(); ++it) {
			std::cout << "error_pages: " << it->first << " " << it->second << std::endl;
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
			std::cout << "\tlocation_path: " << it->locationPath << std::endl;
			std::cout << "\troot: " << it->root << std::endl;
			std::cout << "\tindex: " << it->index << std::endl;
			std::cout << "\tredirect: " << it->redirect << std::endl;
			std::cout << "\tupload_path: " << it->uploadPath << std::endl;
			std::cout << "\tautoindex: " << it->autoindex << std::endl;
			std::cout << "\tupload_enabled: " << it->uploadEnabled << std::endl;
			std::cout << "\tCGI path: " << it->cgiConfig.cgiPath << std::endl;
			std::cout << "\tCGI extension: " << it->cgiConfig.cgiExtension << std::endl;
			std::cout << "\tCGI enabled: " << it->cgiConfig.cgiEnabled << std::endl;
			std::cout << std::endl;
		}
	}

	bool hostIsValid(std::vector<ServerConfigs> &servers) {
		std::vector<int> bytes;
		Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
		for (size_t i = 0; i < servers.size(); ++i)
		{
			if (!inetPton(servers[i].host))
				return false;
			std::istringstream stream(servers[i].host);
			std::string segment;
			int fd = -1;
			uint32_t ip;
			while (std::getline(stream, segment, '.'))
			{
				int byte;
				std::istringstream str(segment);
				str >> byte;
				bytes.push_back(byte);
			}
			ip = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];

			createSocket(fd, AF_INET, SOCK_STREAM, logger);
			sockaddrIn serv_addr;
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = ip;
			serv_addr.sin_port = htons(servers[i].port);
			if (bind(fd, (sockAddr *)&serv_addr, sizeof(serv_addr)) < 0)
			{
				close(fd);
				return false;
			}
			close(fd);
		}
		return true;
	}
}
