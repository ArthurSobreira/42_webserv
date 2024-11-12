#ifndef SERVER_HPP
#define SERVER_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "EpollManager.hpp"

class Server {
	private:
		int	_serverSocket;		// Server main socket
		int	_backlog;			// Maximum number of pending connections
		sockaddr_in _serv_addr;	// Server address structure
		EpollManager &_epoll;	// Epoll manager reference
		ServerConfigs _config;	// Server configurations

	public:
		/* Constructor Method */
		Server( ServerConfigs &config, EpollManager &epoll );

		/* Destructor Method */
		~Server( void );

		/* Public Methods */
		bool	initialize( void );
		int	getServerSocket( void ) const { return _serverSocket; }
		ServerConfigs	&getConfig( void ) { return _config; }

	private:
		bool	_createSocket( void );
		bool	_configureSocket( void );
		bool	_bindSocket( void );
		bool	_listenSocket( void );
		bool	_logErrorAndClose( const std::string &message );
};

#endif