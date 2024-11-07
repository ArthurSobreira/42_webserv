#ifndef WEBSERV_INCLUDES_HPP
#define WEBSERV_INCLUDES_HPP

// C++ Standard Library headers
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <cassert>
#include <limits>

// C headers (for C functions)
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <csignal>

// Standard I/O functions (read, write, close, open)
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

// Error handling functions (strerror, errno)
#include <errno.h>
#include <string.h>

// Networking functions (socket, connect, bind, listen, accept, send, recv)
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Multiplexing functions (select, poll, epoll, kqueue)
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>

// Signal handling functions (signal, kill)
#include <signal.h>

// Directory handling functions (opendir, readdir, closedir)
#include <dirent.h>

// File information functions (stat)
#include <sys/stat.h>

// Process control functions (waitpid, kill)
#include <sys/wait.h>

#endif
