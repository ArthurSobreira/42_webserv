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

			if (line.empty()) { continue; }

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

	bool hostIsValid( const std::string &host ) {
		std::istringstream stream(host);
		std::vector<int> bytes;
		std::string segment;
		int fd = -1;
		uint32_t ip;

		if (!inetPton(host)) { return false; }

		while (std::getline(stream, segment, '.')) {
			int byte;
			std::istringstream str(segment);
			str >> byte;
			bytes.push_back(byte);
		}
		ip = htonl((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]);

		createSocket(fd, AF_INET, SOCK_STREAM);
		sockaddrIn serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = ip;
		serv_addr.sin_port = htons(19000);
		if (bind(fd, (sockAddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			close(fd);
			return (false);
		}
		close(fd);
		return (true);
	}

	bool	isRepeatedMethod( std::vector<httpMethod> &methodsVector, 
		httpMethod method ) {
		if (std::find(methodsVector.begin(), methodsVector.end(), method) 
			!= methodsVector.end()) {
			return true;
		} else { return false; }
	}

	bool	directoryExists( const std::string &path ) {
		struct stat info;
		if (stat(path.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
			return false;
		} else { return true; }
	}

	bool	fileExists( const std::string &path ) {
		struct stat info;
		if (stat(path.c_str(), &info) != 0 || !(info.st_mode & S_IFREG)) {
			return false;
		} else { return true; }
	}

	void	formatPath( std::string &path ) {
		if (path[0] == '/') {
			if (!ConfigUtils::directoryExists(path)) { 
				path = "." + path;
			}
		} else if (path[0] != '.' && path[1] != '/') {
			path = "./" + path;
		}
	}

	void	validateFullLocationPath( LocationConfigs &location ) {
		if (location.cgiEnabled || location.redirectSet) { return ; }

		std::string effectiveRoot;
		if (location.rootSet) { 
			effectiveRoot = location.root; 
		} else { effectiveRoot = DEFAULT_ROOT; }
		
		std::string fullPath = effectiveRoot + location.locationPath;
		if (!ConfigUtils::directoryExists(fullPath)) {
			throw std::runtime_error(ERROR_INVALID_LOCATION_PATH);
		}
	}

	void	validateFullCGIPath( LocationConfigs &location ) {
		if (!location.cgiEnabled) { return ; }
	
		if (location.cgiPath.empty()) {
			throw std::runtime_error(ERROR_INVALID_CGI_PATH);
		}
		std::string cgiPath = location.cgiPath;
		size_t	lastDot = cgiPath.find_last_of('.');
		if (cgiPath.substr(lastDot) != location.cgiExtension) {
			throw std::runtime_error(ERROR_INVALID_CGI_EXTENSION);
		}

		std::string effectiveRoot;
		if (location.rootSet) { 
			effectiveRoot = location.root; 
		} else { effectiveRoot = DEFAULT_ROOT; }

		std::string fullPath = effectiveRoot + "/" + cgiPath;
		if (!ConfigUtils::fileExists(fullPath)) {
			throw std::runtime_error(ERROR_INVALID_CGI_PATH);
		}
	}

	void	createUploadFolder( std::string &uploadPath ) {
		if (!ConfigUtils::directoryExists(uploadPath)) {
			if (mkdir(uploadPath.c_str(), 0777) == -1 && errno != EEXIST) {
				throw std::runtime_error(ERROR_INVALID_UPLOAD_PATH);
			}
		}
	}
}
