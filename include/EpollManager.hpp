
#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

// Class declaration
#include "Logger.hpp"
#include "Fds.hpp"

// Classe encapsulando a lógica de Epoll
class EpollManager
{
	public:
		EpollManager(Logger &logger);
		~EpollManager();
		EpollManager &operator=(const EpollManager &rhs);
		bool addToEpoll(int sockfd, uint32_t events);
		bool removeFromEpoll(int sockfd);
		bool modifyEpoll(int sockfd, uint32_t events);
		int getEpollFD() const;

	private:
		int _epoll_fd;
		Logger &_logger;
		void initialize();

};

#endif // EPOLLMANAGER_HPP
