
#ifndef EPOLLMANAGER_HPP_
#define EPOLLMANAGER_HPP_

// Class declaration
#include "Logger.hpp"

// Classe encapsulando a lógica de Epoll
class EpollManager
{
	public:
		EpollManager();
		~EpollManager();
		void addToEpoll(int sockfd);
		int getEpollFD() const;

	private:
		int epoll_fd;
		void initialize();
};

#endif // EPOLLMANAGER_HPP_
