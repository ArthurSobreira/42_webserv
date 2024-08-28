/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/22 18:29:45 by phenriq2          #+#    #+#             */
/*   Updated: 2024/08/28 14:06:32 by phenriq2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"

std::string getStatusMessage(int status_code) {
    switch (status_code) {
        case 200: return " OK\r\n";
        case 404: return " Not Found\r\n";
        case 500: return " Internal Server Error\r\n";
        default:  return " Unknown Status\r\n";
    }
}

void responseCreate(int client_socket, std::string content, std::string contentType, int status_code = 200) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << getStatusMessage(status_code);
    response << "Server: WebServ/1.0\r\n";
    response << "Content-Type: " << contentType << "; charset=UTF-8\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "\r\n";
    response << content;
	getLogger().log(response.str());
    send(client_socket, response.str().c_str(), response.str().size(), 0);
    close(client_socket);
}


std::string readFile(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return "";
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

std::string getContentType(const std::string &path)
{
	getLogger().log("getContentType" + path);
	if (path.find(".html") != std::string::npos)
		return "text/html";
	if (path.find(".css") != std::string::npos)
		return "text/css";
	if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos)
		return "image/jpeg";
	if (path.find(".png") != std::string::npos)
		return "image/png";
	if (path.find(".gif") != std::string::npos)
		return "image/gif";
	return "text/plain";
}

void pathToFullPath(std::string &path)
{
	if (path == "/")
		path = "/index.html";
	path = "static" + path;
}

void serverLoop(int sockfd)
{
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if (newsockfd < 0)
	{
		getLogger().log("Erro ao aceitar conexão");
		ft_error("Erro ao aceitar conexão", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	getLogger().log("Conexão aceita");
	char buffer[4096];
	recv(newsockfd, buffer, 4096, 0);
	std::string request(buffer);
	std::istringstream request_stream(request);
	std::string method, path, protocol;
	request_stream >> method >> path >> protocol;
	std::ostringstream log;
	log << "Requisição recebida: " << method << " " << path << " " << protocol;
	getLogger().log(log.str());
	if (method == "GET")
	{
		pathToFullPath(path);
		std::cout << path << std::endl;
		std::string content = readFile(path);
		if (content.empty())
		{
			getLogger().log("Arquivo não encontrado");
			path = "static/404.html";
			responseCreate(newsockfd, getError404().getBody(), getContentType(path), 404);
		}
		else
		{
			responseCreate(newsockfd, content, getContentType(path));
			getLogger().log("Arquivo enviado com sucesso");
		}
	}
}

int serverInit(std::string ip, int port)
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	if (!inetPton(ip, serv_addr.sin_addr.s_addr))
		ft_error("Erro ao converter endereço IP", __FUNCTION__, __FILE__, __LINE__, std::invalid_argument("byte inválido"));
	serv_addr.sin_port = htons(port);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ft_error("Erro ao criar socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    ft_error("Erro ao definir opção de socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		close(sockfd);
		getLogger().log("Conexão fechada por falha no bind");
		ft_error("Erro ao fazer bind", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	if (listen(sockfd, SOMAXCONN) < 0)
	{
		close(sockfd);
		getLogger().log("Conexão fechada por falha no listen");
		ft_error("Erro ao fazer listen", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	getLogger().log("Servidor iniciado com sucesso");
	return sockfd;
}

void signals(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM)
	{
		getLogger().log("Sinal de interrupção recebido");
		throw std::runtime_error("bye bye");
	}
}

int main(int argc, char **argv)
{
	std::vector<int> arr;
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config file> " << std::endl;
		return 1;
	}
	try
	{
		signal(SIGINT, signals);
		signal(SIGQUIT, signals);
		arr.push_back(serverInit("10.11.9.1", 13000));
		while (1)
			serverLoop(arr[0]);
	}
	catch (std::runtime_error &e)
	{
		for (std::vector<int>::iterator it = arr.begin(); it != arr.end(); it++)
			close(*it);
		std::cerr << e.what() << std::endl;
		return 0;
	}
	catch (std::exception &e)
	{
		return 1;
	}
	return 0;
}