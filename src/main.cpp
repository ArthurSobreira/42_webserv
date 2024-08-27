/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/22 18:29:45 by phenriq2          #+#    #+#             */
/*   Updated: 2024/08/27 18:36:16 by phenriq2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"

void serverInit(std::string ip, int port)
{
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	if (inetPton(ip, serv_addr.sin_addr.s_addr))
		ft_error("Erro ao converter endereço IP", __FUNCTION__, __FILE__, __LINE__, std::invalid_argument("byte inválido"));
	serv_addr.sin_port = htons(port);
	// int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// if (sockfd < 0)
	// 	ft_error("Erro ao criar socket", strerror(errno), __FUNCTION__, __FILE__, __LINE__, errno);
	// if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 || 1==1)
	// {
	// 	ft_error("Erro ao fazer bind", strerror(errno), __FUNCTION__, __FILE__, __LINE__, errno, std::
	// 	close(sockfd);
	// }
	// if(listen(sockfd, SOMAXCONN) < 0)
	// {
	// 	ft_error("Erro ao fazer listen", strerror(errno), __FUNCTION__, __FILE__, __LINE__, errno);
	// 	close(sockfd);
	// }
}

int main(int argc, char **argv)
{
	Logger log("server.log");
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config file> " << std::endl;
		return 1;
	}
	try
	{
		serverInit("127.0.0.1", 13000);
	}
	catch (std::exception &e)
	{
		return 1;
	}
	return 0;
}