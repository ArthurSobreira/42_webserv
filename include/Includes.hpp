#ifndef WEBSERV_INCLUDES_HPP
#define WEBSERV_INCLUDES_HPP

// C++ Standard Library headers
#include <iostream>	 // Entrada e saída padrão
#include <string>	 // Manipulação de strings
#include <sstream>	 // Stream de strings
#include <fstream>	 // Manipulação de arquivos
#include <map>		 // Containers de mapa associativo
#include <set>		 // Containers de conjunto
#include <vector>	 // Containers de vetor
#include <stdexcept> // Manipulação de exceções
#include <algorithm> // Funções de algoritmo padrão
#include <ctime>	 // Manipulação de tempo
#include <cassert>   // Macros de asserção
#include <limits>	 // Limites numéricos

// C headers
#include <cstring> // Manipulação de strings em C (ex: strlen, strcpy)
#include <cstdlib> // Funções de utilitários gerais (ex: malloc, free)
#include <cstdio>  // Entrada e saída em C (ex: printf, fopen)

// Standard I/O functions (read, write, close, open)
#include <unistd.h>	   // close, read, write, access, dup, dup2, execve, fork, pipe, chdir
#include <fcntl.h>	   // open, fcntl
#include <sys/types.h> // stat, waitpid, opendir, readdir, closedir

// Error handling functions (strerror, errno)
#include <errno.h>	// errno
#include <string.h> // strerror, gai_strerror

// Networking functions (socket, connect, bind, listen, accept, send, recv)
#include <sys/socket.h> // socket, bind, listen, accept, send, recv, socketpair, setsockopt, getsockname
#include <netinet/in.h> // htons, htonl, ntohs, ntohl
#include <netdb.h>		// getaddrinfo, freeaddrinfo, getprotobyname

// Multiplexing functions (select, poll, epoll, kqueue)
#include <sys/select.h> // select
#include <poll.h>		// poll
#include <sys/epoll.h>	// epoll_create, epoll_ctl, epoll_wait

// Signal handling functions (signal, kill)
#include <signal.h> // signal, kill

// Directory handling functions (opendir, readdir, closedir)
#include <dirent.h> // opendir, readdir, closedir

// File information functions (stat)
#include <sys/stat.h> // stat

// Process control functions (waitpid, kill)
#include <sys/wait.h> // waitpid

// Defines

#include "Defines.hpp"

#endif
