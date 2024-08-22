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
#include <sys/types.h>    // Tipos de dados para sockets e outras funções
#include <sys/socket.h>   // Funções de manipulação de sockets
#include <netinet/in.h>   // Estruturas de endereço de internet
#include <arpa/inet.h>    // Funções de conversão de endereços IP
#include <unistd.h>       // Funções POSIX (ex: fork, exec, close)
#include <poll.h>         // Multiplexação de I/O (poll)
#include <fcntl.h>        // Controle de arquivos (ex: O_NONBLOCK)
#include <sys/wait.h>     // Espera por processos (ex: waitpid)
#include <errno.h>        // Definições de códigos de erro

// Custom project headers (headers próprios do projeto)
#include "server.hpp"     // Declarações da classe do servidor
#include "request.hpp"    // Declarações para o tratamento de requisições
#include "response.hpp"   // Declarações para o tratamento de respostas
#include "cgi_handler.hpp" // Declarações para o gerenciamento de CGI
#include "utils.hpp"      // Funções utilitárias e macros


#endif // WEBSERV_INCLUDES_HPP
