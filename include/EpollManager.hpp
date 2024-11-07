
#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Fds.hpp"

class EpollManager {
	private:
		int	_epoll_fd;
		Logger	&_logger;

	public:
		/* Constructor Method */
		EpollManager( Logger &logger );

		/* Destructor Method */
		~EpollManager( void );

		/* Public Methods */
		bool	addToEpoll( int sockfd, uint32_t events );
		bool	removeFromEpoll( int sockfd );
		bool	modifyEpoll( int sockfd, uint32_t events );
		int	getEpollFD( void ) const;

	private:
		void	_initialize( void );
};

#endif // EPOLLMANAGER_HPP
