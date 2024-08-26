#ifndef WEBSERV_INCLUDES_HPP
#define WEBSERV_INCLUDES_HPP

// C++ Standard Library headers
#include <iostream>       // Entrada e saída padrão
#include <string>         // Manipulação de strings
#include <sstream>        // Stream de strings
#include <fstream>        // Manipulação de arquivos
#include <map>            // Containers de mapa associativo
#include <vector>         // Containers de vetor
#include <stdexcept>      // Manipulação de exceções
#include <algorithm>      // Funções de algoritmo padrão

// C headers
#include <cstring>        // Manipulação de strings em C (ex: strlen, strcpy)
#include <cstdlib>        // Funções de utilitários gerais (ex: malloc, free)
#include <cstdio>         // Entrada e saída em C (ex: printf, fopen)

// POSIX headers (para sistemas UNIX/Linux)
#include <sys/types.h>       // Tipos básicos de dados e definições de tipo
#include <sys/stat.h>        // Definições para permissões de arquivos e outras operações de arquivos
#include <sys/socket.h>      // Definições de funções e estruturas para comunicação de sockets
#include <sys/wait.h>        // Definições para controle de processos (waitpid)
#include <sys/epoll.h>       // Definições para o mecanismo de epoll (epoll_create, epoll_ctl, epoll_wait)
#include <sys/select.h>      // Definições para a multiplexação de entrada/saída (select)
#include <sys/poll.h>        // Definições para a multiplexação de entrada/saída (poll)
#include <fcntl.h>           // Definições para controle de arquivos (fcntl)
#include <unistd.h>          // Definições de funções do sistema POSIX (execve, dup, dup2, pipe, fork, close, read, write, chdir, access)
#include <errno.h>           // Definições para códigos de erro (errno, strerror)
#include <string.h>          // Funções de manipulação de strings (strerror, gai_strerror)
#include <netinet/in.h>      // Definições de funções e estruturas para comunicação de rede (htons, htonl, ntohs, ntohl)
#include <arpa/inet.h>       // Definições para conversões de endereços IP (inet_pton, inet_ntop)
#include <netdb.h>           // Definições para a interface de banco de dados de rede (getaddrinfo, freeaddrinfo, getprotobyname)
#include <dirent.h>          // Definições para manipulação de diretórios (opendir, readdir, closedir)
#include <stdio.h>           // Definições para entrada e saída padrão (printf, scanf, etc. – não estritamente necessário para as funções listadas, mas geralmente incluído)
#include <stdlib.h>          // Funções gerais da biblioteca padrão (malloc, free – não estritamente necessário para as funções listadas, mas geralmente incluído)
#include <signal.h>          // Definições para sinais (kill, signal)



// Custom project headers (headers próprios do projeto)
#include "server.hpp"     // Declarações da classe do servidor
#include "request.hpp"    // Declarações para o tratamento de requisições
#include "response.hpp"   // Declarações para o tratamento de respostas
#include "cgi_handler.hpp" // Declarações para o gerenciamento de CGI
#include "utils.hpp"      // Funções utilitárias e macros


#endif // WEBSERV_INCLUDES_HPP
