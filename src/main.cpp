/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arsobrei <arsobrei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/22 18:29:45 by phenriq2          #+#    #+#             */
/*   Updated: 2024/08/23 17:51:02 by arsobrei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"

void ft_error(const char *message, const char *error_detail, const char *function, const char *file, int line, int error_code)
{
	std::cerr << "Erro: " << message << " (" << error_detail << ")"
			  << " in function " << function << " at " << file << ":" << line
			  << " Error code: " << error_code << std::endl;
}

int main(void)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		ft_error("Erro ao criar socket", strerror(errno), __FUNCTION__, __FILE__, __LINE__, errno);
		return 1;
	}
	std::cout << "Socket criado com sucesso!" << std::endl;

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY; // Vincula a qualquer endereço disponível
	server_address.sin_port = htons(13000);		 // Porta 8080

	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		perror("bind failed");
		close(server_socket);
		return 1;
	}

	if (listen(server_socket, SOMAXCONN) < 0)
	{
		perror("listen failed");
		close(server_socket);
		return 1;
	}

	std::cout << "Servidor escutando na porta 8080..." << std::endl;

	while (true)
	{
		struct sockaddr_in client_address;
		socklen_t client_len = sizeof(client_address);
		int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

		if (client_socket < 0)
		{
			perror("accept failed");
			close(server_socket);
			return 1;
		}

		std::cout << "Cliente conectado!" << std::endl;

		const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, Telnet!\n";
		send(client_socket, response, strlen(response), 0);

		close(client_socket);
		std::cout << "Conexão com o cliente fechada." << std::endl;
	}

	close(server_socket);
	return 0;
}
