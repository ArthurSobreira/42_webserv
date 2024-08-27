/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/22 18:29:45 by phenriq2          #+#    #+#             */
/*   Updated: 2024/08/27 19:22:08 by phenriq2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"

void responseCreate(int client_socket, std::string content, int status_code = 200)
{
	std::ostringstream response;
	response << "HTTP/1.1 " << status_code << " OK\r\n";
	response << "Server: WebServ/1.0\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << content.size() << "\r\n";
	response << "\r\n";
	response << content;
	send(client_socket, response.str().c_str(), response.str().size(), 0);
	close(client_socket);
}

void serverLoop(int sockfd)
{
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	std::cout << cli_addr.sin_addr.s_addr << std::endl;
	if (newsockfd < 0)
	{
		getLogger().log("Erro ao aceitar conexão");
		ft_error("Erro ao aceitar conexão", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	getLogger().log("Conexão aceita");
	char buffer[4096];
	bzero(buffer, 4096);
	int n = read(newsockfd, buffer, 4096);
	if (n < 0)
	{
		getLogger().log("Erro ao ler do socket");
		ft_error("Erro ao ler do socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	getLogger().log("Mensagem recebida: " + std::string(buffer));
	std::ifstream file("static/index.html");
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	responseCreate(newsockfd, content);
	close(newsockfd);
	getLogger().log("Conexão fechada");
}

void serverInit(std::string ip, int port)
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	if (!inetPton(ip, serv_addr.sin_addr.s_addr))
		ft_error("Erro ao converter endereço IP", __FUNCTION__, __FILE__, __LINE__, std::invalid_argument("byte inválido"));
	serv_addr.sin_port = htons(port);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ft_error("Erro ao criar socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		close(sockfd);
		getLogger().log("Conexão fechada por falha no bind");
		ft_error("Erro ao fazer bind", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	if(listen(sockfd, SOMAXCONN) < 0)
	{
		close(sockfd);
		getLogger().log("Conexão fechada por falha no listen");
		ft_error("Erro ao fazer listen", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	}
	getLogger().log("Servidor iniciado com sucesso");
	while (1)
	{
		serverLoop(sockfd);
	}
}

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config file> " << std::endl;
		return 1;
	}
	try
	{
		serverInit("127.0.0.1", 13001);
	}
	catch (std::exception &e)
	{
		return 1;
	}
	return 0;
}