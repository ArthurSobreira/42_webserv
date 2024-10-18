#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

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

	std::string	intToString(int value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	void	deleteEnvp(char **envp) {
		for (int i = 0; envp[i] != NULL; ++i) {
			delete[] envp[i];
		}
		delete[] envp;
	}
}
