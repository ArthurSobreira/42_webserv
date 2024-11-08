#include "EpollManager.hpp"
#include "Includes.hpp"
#include "Defines.hpp"
#include "Utils.hpp"

EpollManager::EpollManager(Logger &logger)
    : _epoll_fd(-1), _logger(logger) { 
    _initialize(); 
}

void EpollManager::_initialize( void )
{
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1)
        throw std::runtime_error("Failed to create epoll instance");
}

int EpollManager::getEpollFD() const { return _epoll_fd; }

bool EpollManager::addToEpoll(int sockfd, uint32_t events)
{
    epoll_event event = {};
    event.events = events;
    event.data.fd = sockfd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
    {
        close(sockfd);
        return false;
    }
    // std::cout << "Added fd to epoll: " << sockfd << " with events: " << events << std::endl;
    return true;
}

bool EpollManager::modifyEpoll(int sockfd, uint32_t events)
{
    epoll_event event = {};
    event.events = events;
    event.data.fd = sockfd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, sockfd, &event) == -1)
    {
        _logger.logError(LOG_ERROR, "Failed to modify client socket to EPOLLOUT");
        close(sockfd);
        return false;
    }
    // std::cout << "Modified fd in epoll: " << sockfd << " with events: " << events << std::endl;
    return true;
}

bool EpollManager::removeFromEpoll(int sockfd)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, sockfd, NULL) == -1)
    {
        close(sockfd);
        return false;
    }
    // std::cout << "Removed fd from epoll: " << sockfd << std::endl;
    return true;
}

EpollManager::~EpollManager()
{
    if (_epoll_fd != -1)
        close(_epoll_fd);
}
