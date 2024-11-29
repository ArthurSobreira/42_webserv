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

struct ClientData {
    std::string request;
    std::string response;
    bool connection;
    bool completeRequest;
    size_t contentLength;
    size_t bytesRead;

	/* Default Constructor */
	ClientData( void ) : request(DEFAULT_EMPTY), 
		response(DEFAULT_EMPTY), connection(false), 
		completeRequest(false), contentLength(0), bytesRead(0) {};

	/* Struct Constructor */
	ClientData( const std::string &req, const std::string &res, 
		bool conn, bool compReq, size_t contentLen, size_t bRead );
};

class ServerManager {
	private:
		EpollManager _epollManager;
		Config _config;
		Fds _fds;
		std::vector<Server*> _servers; 
		std::map<int, int> _clientServerMap;
		std::map<int, ClientData> _clientDataMap;

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
		void	_acceptConnection( int serverSocket );
		void	_handleRead( int clientSocket );
		void	_handleWrite (int clientSocket );
		void	_handleResponse( Request &request, ServerConfigs &server, 
			int clientSocket );
		void	_getContentLength( int clientSocket, std::string &buffer );
		void	_restartStruct( ClientData &data );
		void	_closeConnection( int clientSocket );
		void	_handleError( int clientSocket, const std::string &errorPage, 
			const std::string &status, const std::string &error_message );
};

#endif // SERVERMANAGER_HPP