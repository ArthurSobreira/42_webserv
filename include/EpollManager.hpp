
#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Utils.hpp"
#include "Fds.hpp"

class EpollManager {
	private:
		int	_epoll_fd;

	public:
		/* Constructor Method */
		EpollManager( void );

		/* Destructor Method */
		~EpollManager( void );

		/* Public Methods */
		int	getEpollFD( void ) const;
		bool	addToEpoll( int sockfd, uint32_t events );
		bool	modifyEpoll( int sockfd, uint32_t events );
		bool	removeFromEpoll( int sockfd );

	private:
		void	_initialize( void );
};

#endif // EPOLLMANAGER_HPP
