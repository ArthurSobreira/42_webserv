#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include <filesystem>

/* Server Extraction Functions */
namespace ServerExtraction {
	void	port( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		std::stringstream stringPort(tokens[1]);
		long long portValue;
		if (stringPort >> portValue) {
			if (portValue > std::numeric_limits<unsigned short>::max() || 
				portValue < 1024) {
				throw std::runtime_error(ERROR_INVALID_PORT);
			}
			server.port = static_cast<unsigned short>(portValue);
		} else {
			throw std::runtime_error(ERROR_INVALID_PORT);
		}
	}

	void	host( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		server.host = tokens[1];
		if (!ConfigUtils::hostIsValid(server)) {
			throw std::runtime_error(ERROR_INVALID_HOST);
		}
	}

	void	serverName( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		} else { server.serverName = tokens[1]; }
	}

	void	limitBodySize( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		std::stringstream stringSize(tokens[1]);
		size_t	limitBodySizeMB;
		if (stringSize >> limitBodySizeMB) {
			server.limitBodySize = limitBodySizeMB * 1024 * 1024;
			if (tokens[1][0] == '-') {
				throw std::runtime_error(ERROR_INVALID_LIMIT_BODY_SIZE);
			}
		} else {
			throw std::runtime_error(ERROR_INVALID_LIMIT_BODY_SIZE);
		}
	}

	void	errorPages( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 3 || tokens[1].empty() || tokens[2].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		std::string	errorCode = tokens[1];
		std::string	fileName = tokens[2];
		ConfigUtils::formatPath(fileName);
		std::ifstream file(fileName.c_str());

		if (!file.is_open() || file.fail()) {
			throw std::runtime_error(ERROR_INVALID_ERROR_PAGE);
		}
		server.errorPages[errorCode] = fileName;
	}
}

/* Location Extraction Functions */
namespace LocationExtraction {
	void	methods( stringVector &tokens, LocationConfigs &location ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		stringVector::iterator it;

		location.methods.clear();
		for (it = tokens.begin() + 1; it != tokens.end(); ++it) {
			std::transform(it->begin(), it->end(), it->begin(), ::toupper);
			if (*it == "GET" && 
				!ConfigUtils::isRepeatedMethod(location.methods, GET)) {
				location.methods.push_back(GET);
			} else if (*it == "POST" && 
				!ConfigUtils::isRepeatedMethod(location.methods, POST)) {
				location.methods.push_back(POST);
			} else if (*it == "DELETE" && 
				!ConfigUtils::isRepeatedMethod(location.methods, DELETE)) {
				location.methods.push_back(DELETE);
			} else { throw std::runtime_error(ERROR_INVALID_METHOD); }
		}
	}

	void	locationPath( stringVector &tokens, LocationConfigs &location ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		} else { location.locationPath = tokens[1]; }
	}

	void	root( stringVector &tokens, LocationConfigs &location ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		std::string	rootPath = tokens[1];
		ConfigUtils::formatPath(rootPath);
		if (!ConfigUtils::directoryExists(rootPath)) {
			throw std::runtime_error(ERROR_INVALID_ROOT);
		} else { 
			location.root = rootPath;
			location.rootSet = true;	
		}
	}

	void	index( stringVector &tokens, LocationConfigs &location ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		} else { location.index = tokens[1]; }
	}

	void	redirect( stringVector &tokens, LocationConfigs &location ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		} else { 
			location.redirect = tokens[1]; 
			location.redirectSet = true;
		}
	}

	void	uploadPath( stringVector &tokens, LocationConfigs &location ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		std::string	uploadPath = tokens[1];
		ConfigUtils::formatPath(uploadPath);
		if (!ConfigUtils::directoryExists(uploadPath)) {
			throw std::runtime_error(ERROR_INVALID_UPLOAD_PATH);
		} else { location.uploadPath = uploadPath; }
	}
}
