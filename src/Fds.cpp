#include "Fds.hpp"

Fds::Fds() {}

Fds::~Fds() {}

void Fds::addFdToServer(int fd)
{
	_serverFds.push_back(fd);
}

bool Fds::isFdInServer(int fd)
{
	for (std::vector<int>::iterator it = _serverFds.begin(); it != _serverFds.end(); ++it)
	{
		if (*it == fd)
			return true;
	}
	return false;
}
