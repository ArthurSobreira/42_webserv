#include "Server.hpp"

/* Constructor Method */
Server::Server( ServerConfigs &config, EpollManager &epoll )
	: _epoll(epoll), _config(config) {
	_backlog = SOMAXCONN;
	_serverSocket = -1;
}

/* Destructor Method */
Server::~Server( void ) {
	if (_serverSocket != -1)
		close(_serverSocket);
}

/* Public Method */
bool Server::initialize( void ) { 
	if (!_createSocket()) return false;
	if (!_configureSocket()) return false;
	if (!_bindSocket()) return false;
	if (!_listenSocket()) return false;
	if(!_epoll.addToEpoll(_serverSocket, EPOLLIN)) return false;
	return true;
}

/* Private Methods */
bool Server::_createSocket( void ) {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		logger.logError(LOG_ERROR, "Error opening socket");
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Socket created");
	return true;
}

bool Server::_configureSocket( void ) {
	int opt = 1;
	logStream log;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		log << "Error on setting socket options";
		return _logErrorAndClose(log.str());
	}
	log << "Socket options set";
	logger.logDebug(LOG_DEBUG, log.str());
	return true;
}

bool Server::_bindSocket( void ) {
	logStream log;
	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	_serv_addr.sin_port = htons(_config.port);
	if (bind(_serverSocket, (sockAddr *)&_serv_addr, sizeof(_serv_addr)) < 0)
	{
		log << "Error on binding to port " << _config.port;
		return _logErrorAndClose(log.str());
	}
	log << "Binded to port " << _config.port;
	logger.logDebug(LOG_DEBUG, log.str());
	return true;
}

bool Server::_listenSocket( void ) {
	logStream log;
	if (listen(_serverSocket, _backlog) < 0)
	{
		log << "Error on listening to port " << _config.port;
		return _logErrorAndClose(log.str());
	}
	log << "Listening on port " << _config.port;
	logger.logDebug(LOG_DEBUG, log.str());
	return true;
}

bool Server::_logErrorAndClose( const std::string &message ) {
	close(_serverSocket);
	logger.logError(LOG_ERROR, message);
	return false;
}
