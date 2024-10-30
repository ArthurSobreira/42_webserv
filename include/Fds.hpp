#ifndef FDS_HPP
#define FDS_HPP
#include "Includes.hpp"
class Fds
{
public:
	Fds();
	~Fds();
	void addFdToServer(int fd);
	bool isFdInServer(int fd);

private:
	std::vector<int> _serverFds;
};

#endif // FDS_HPP