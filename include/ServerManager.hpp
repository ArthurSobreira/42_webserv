#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "Server.hpp"
#include "EpollManager.hpp"
#include "Fds.hpp"
#include "Request.hpp"
#include "GetResponse.hpp"
#include "PostResponse.hpp"
#include "DeleteResponse.hpp"
#include "CGIResponse.hpp"

class ServerManager {
	private:
		EpollManager _epollManager;
		Config _config;
		Fds _fds;
		std::vector<Server*> _servers;
		std::map<int, std::string> _requestMap;
		std::map<int, std::string> _responseMap;
		std::map<int, bool> _connectionMap; 
		std::map<int, int> _clientServerMap;

	public:
		/* Constructor Method */
		ServerManager( const std::string &configFilePath );

		/* Destructor Method */
		~ServerManager( void );

		/* Public Method */
		void	run( void );

	private:
		bool	_initializeServers( void );
		void	_handleEvents( void );
		void	_acceptConnection(int serverSocket);
		void	_handleRead( int clientSocket );
		void	_handleWrite (int clientSocket );
		void	_handleResponse(Request &request, ServerConfigs &server, 
			int clientSocket);
		bool	_verifyContentLength(int clientSocket, std::string &buffer);
		void	_closeConnection(int clientSocket);
		void	_handleError(int clientSocket, const std::string &errorPage, 
			const std::string &status);
};

#endif // SERVERMANAGER_HPP