/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 11:55:07 by phenriq2          #+#    #+#             */
/*   Updated: 2024/09/05 19:28:18 by phenriq2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"

void clientServer(int sockfd, Logger &logger){
	sockaddrIn cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (sockAddr*)&cli_addr, &clilen);
	if (newsockfd < 0)
	{
		logger.logError("ERROR", "Error on accept");
		return;
	}
	std::ostringstream log;
	log << "Accepted connection from " << inetNtop(cli_addr.sin_addr.s_addr) << ":" << ntohs(cli_addr.sin_port);
	logger.logDebug(log.str());
	
	close(newsockfd);
}




void createServer(int &sockfd, int &port, uint32_t &ip, int &backlog, Logger &logger)
{
	sockaddrIn serv_addr;
	std::ostringstream log;
	if (!createSocket(sockfd, AF_INET, SOCK_STREAM, logger))
		ft_error("Error creating socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error creating socket"));
	if (!bindSocket(sockfd, port, ip, serv_addr, logger))
	{
		close(sockfd);
		ft_error("Error binding socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error binding socket"));
	}
	if (!listenSocket(sockfd, backlog, logger))
	{
		close(sockfd);
		ft_error("Error listening on socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error listening on socket"));
	}
	log<<"Server created on "<<inetNtop(ip)<<":"<<port;
	logger.logDebug(log.str());
}


void populateServersAndPorts(std::string config_file_path, std::vector<uint32_t> &servers, std::vector<int> &ports, Logger &logger)
{
	(void) config_file_path;
	std::string server1 = "127.0.0.1";
	std::string server2 = "10.11.9.1";
	uint32_t ip1;
	uint32_t ip2;
	
	int porta1 = 13000;
	int porta2 = 13001;
	if (!inetPton(server1, ip1, logger))
		ft_error("Error converting IP to binary format", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error converting IP to binary format"));
	if (!inetPton(server2, ip2, logger))
		ft_error("Error converting IP to binary format", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Error converting IP to binary format"));
	servers.push_back(ip1);
	servers.push_back(ip2);
	ports.push_back(porta1);
	ports.push_back(porta2);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return 1;
	}
	std::string config_file_path = argv[1];
	std::vector<uint32_t> servers;
	std::vector<int> ports;
	std::vector<int> fds;
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	try{
		populateServersAndPorts(config_file_path, servers, ports, logger);
		for (size_t i = 0; i < servers.size(); i++)
		{
			int sockfd = -1;
			int backlog = SOMAXCONN;
			uint32_t ip = servers[i];
			createServer(sockfd, ports[i], ip, backlog, logger);
			fds.push_back(sockfd);
		}
		while(true)
			for (size_t i = 0; i < fds.size(); i++)
				clientServer(fds[i], logger);
		for (size_t i = 0; i < fds.size(); i++)
			close(fds[i]);
	}
	catch(const std::exception &e)
	{
		ft_error("Error creating server", __FUNCTION__, __FILE__, __LINE__, e);
	}

	std::cout << GREEN << "MAIN" << RESET << std::endl;

	return 0;
}