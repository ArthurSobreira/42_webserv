#include "EpollManager.hpp"
#include "Includes.hpp"
#include "Defines.hpp"
#include "Utils.hpp"

/* Constructor Method */
EpollManager::EpollManager( void ) : _epoll_fd(-1) { 
    _initialize(); 
}

/* Destructor Method */
EpollManager::~EpollManager( void ) {
    if (_epoll_fd != -1)
        close(_epoll_fd);
}

void EpollManager::_initialize( void ) {
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1) {
        throw std::runtime_error("Failed to create epoll instance");
    }
}

/* Public Methods */
int EpollManager::getEpollFD( void ) const { return _epoll_fd; }

bool EpollManager::addToEpoll( int sockfd, uint32_t events ) {
    epoll_event event = {};
    event.events = events;
    event.data.fd = sockfd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
    {
        close(sockfd);
        return false;
    }
    return true;
}

bool EpollManager::modifyEpoll( int sockfd, uint32_t events ) {
    epoll_event event = {};
    event.events = events;
    event.data.fd = sockfd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, sockfd, &event) == -1) {
        logger.logError(LOG_ERROR, "Failed to modify client socket to EPOLLOUT");
        close(sockfd);
        return false;
    }
    return true;
}

bool EpollManager::removeFromEpoll( int sockfd ) {
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, sockfd, NULL) == -1)
    {
        close(sockfd);
        return false;
    }
    return true;
}
