#include "EpollManager.hpp"
#include "Includes.hpp"

EpollManager::EpollManager() : epoll_fd(-1) { initialize(); }

void EpollManager::initialize()
{
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		throw std::runtime_error("Failed to create epoll instance");
}

int EpollManager::getEpollFD() const{ return epoll_fd; }

void EpollManager::addToEpoll(int sockfd)
{
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
		throw std::runtime_error("Failed to add socket to epoll");
}

EpollManager::~EpollManager()
{
	if (epoll_fd != -1)
		close(epoll_fd);
}
