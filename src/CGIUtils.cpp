#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Request.hpp"

namespace CGIUtils {
	bool	methodIsOnLocation( LocationConfigs &location, 
		const std::string &method ) {
		std::vector<httpMethod> methods = location.methods;

		if (methods.empty()) { return true; }
		for (std::vector<httpMethod>::iterator it = methods.begin(); 
			it != methods.end(); ++it) {
			if (*it == GET && method == "GET") { return true; }
			if (*it == POST && method == "POST") { return true; }
			if (*it == DELETE && method == "DELETE") { return true; }
		}
		return (false);
	}

	bool	isUploadRequest(const Request &request) {
		if (request.getMethod() == "POST") {
			std::string contentType = request.getHeader("Content-Type");
			if (contentType.find("multipart/form-data") != std::string::npos) {
				return (true);
			}

			std::string contentDisposition = 
				request.getHeader("Content-Disposition");
			if (contentDisposition.find("filename=") != std::string::npos) {
				return (true);
			}
		}
		return (false);
	}

	std::string extractFileName( const std::string &body ) {
		std::string filename;
		std::string::size_type pos = body.find("filename=\"");
		if (pos != std::string::npos) {
			pos += 10;
			std::string::size_type endPos = body.find("\"", pos);
			if (endPos != std::string::npos) {
				filename = body.substr(pos, endPos - pos);
			}
		}
		return (filename);
	}

	std::string	intToString( int value ) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	void	deleteEnvp( char **envp ) {
		for (int i = 0; envp[i] != NULL; ++i) {
			delete[] envp[i];
		}
		delete[] envp;
	}
}
