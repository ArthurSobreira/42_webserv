#ifndef SERVER_HPP
#define SERVER_HPP

#include "Includes.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "EpollManager.hpp"
#include <vector>
#include <sstream>

class Logger;

class Server
{
public:
	Server(ServerConfigs &config, Logger &logger, EpollManager &epoll);
	~Server();
	bool initialize();
	int getServerSocket() const { return _serverSocket; }
	ServerConfigs &getConfig() { return _config; }

private:
	// Métodos de manipulação de sockets e conexões
	bool createSocket();
	bool configureSocket();
	bool bindSocket();
	bool listenSocket();
	bool logErrorAndClose(const std::string &message);

	// Atributos privados
	int _serverSocket;	   // Socket principal do servidor
	int _backlog;			// Tamanho da fila de conexões
	sockaddr_in _serv_addr;   // Estrutura de endereço do servidor
	EpollManager &_epoll;	 // Instância para gerenciar o epoll
	Logger &_logger;		  // Logger para debug e logs de erro
	ServerConfigs _config;   // Configurações específicas do servidor
};

#endif // SERVER_HPP