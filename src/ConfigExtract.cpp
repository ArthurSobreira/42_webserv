#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Config.hpp"

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
		Logger _logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
		uint32_t temp_ip;
		if (!inetPton(tokens[1], temp_ip, _logger)) {
			throw std::runtime_error(ERROR_INVALID_HOST);
		}
		server.host = tokens[1];
	}

	void	serverName( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 2 || tokens[1].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		server.serverName = tokens[1];
	}

	void	limitBodySize( stringVector &tokens, ServerConfigs &server ) {
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

	void	errorPages( stringVector &tokens, ServerConfigs &server ) {
		if (tokens.size() < 3 || tokens[1].empty() || tokens[2].empty()) {
			throw std::runtime_error(ERROR_MISSING_VALUE);
		}
		std::string	errorCode = tokens[1];
		std::string	fileName = tokens[2];
		if (fileName[0] == '/') {
			fileName = "." + fileName;
		} else if (fileName[0] != '.' && fileName[1] != '/') {
			fileName = "./" + fileName;
		}

		std::ifstream file(fileName.c_str());
		if (!file.is_open() || file.fail()) {
			throw std::runtime_error(ERROR_INVALID_ERROR_PAGE);
		}
		server.errorPages[errorCode] = fileName;
	}
}
