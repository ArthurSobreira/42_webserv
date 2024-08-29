/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   main.cpp										   :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: phenriq2 <phenriq2@student.42sp.org.br>	+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2024/08/22 18:29:45 by phenriq2		  #+#	#+#			 */
/*   Updated: 2024/08/29 13:21:39 by phenriq2		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "includes.hpp"

int epoll_fd = epoll_create1(0);

std::string getStatusMessage(int status_code)
{
	switch (status_code)
	{
	case 200:
		return " OK\r\n";
	case 404:
		return " Not Found\r\n";
	case 500:
		return " Internal Server Error\r\n";
	default:
		return " Unknown Status\r\n";
	}
}

void addHeader(std::ostringstream &oss, const std::string &title) {
    oss << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n"
        << "<html>\n<head>\n"
        << "<title>Index of " << title << "</title>\n"
        << "</head>\n<body>\n"
        << "<h1>Index of " << title << "</h1>\n"
        << "<table>\n"
        << "<tr><th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"[ICO]\"></th>"
        << "<th><a href=\"?C=N;O=D\">Name</a></th>"
        << "<th><a href=\"?C=M;O=A\">Last modified</a></th>"
        << "<th><a href=\"?C=S;O=A\">Size</a></th>"
        << "<th><a href=\"?C=D;O=A\">Description</a></th></tr>\n"
        << "<tr><th colspan=\"5\"><hr></th></tr>\n";
}

void addFileEntry(std::ostringstream &oss, const std::string &name, const std::string &modDate, const std::string &size, std::string &icon) {
	if(name.find(".html") != std::string::npos)
		icon = "/icons/html.png";
	else if (name.find(".png") != std::string::npos || name.find(".jpg") != std::string::npos || name.find(".jpeg") != std::string::npos)
		icon = "/icons/image.png";
    oss << "<tr><td valign=\"top\"><img src=\"" << icon << "\" alt=\"[   ]\"></td>"
        << "<td><a href=\"" << name << "\">" << name << "</a></td>"
        << "<td align=\"right\">" << modDate << "</td>"
        << "<td align=\"right\">" << size << "</td>"
        << "<td>&nbsp;</td></tr>\n";
}

void addFooter(std::ostringstream &oss) {
    oss << "<tr><th colspan=\"5\"><hr></th></tr>\n"
        << "</table>\n</body></html>";
}

bool isDirectory(const std::string& path) {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0) 
		return false;
	return S_ISDIR(statbuf.st_mode);
}

std::string listDirectory(const std::string& dirPath) {
	std::string dirIcon = "/icons/folder.gif";
	std::string fileIcon = "/icons/unknown.png";
	DIR* dir = opendir(dirPath.c_str());
	if (!dir) {
		return "<p>Erro ao abrir o diretório.</p>";
	}

	std::ostringstream html;
	addHeader(html, dirPath);

	diretory *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name == ".") 
			continue;
		std::string path = dirPath + "/" + name;
		status statbuf;
		stat(path.c_str(), &statbuf);
		std::string modDate = ctime(&statbuf.st_mtime);
		std::ostringstream size; 
		size << statbuf.st_size;
		if (isDirectory(path))
			addFileEntry(html, name + "/", modDate, size.str(), dirIcon);
		else 
			addFileEntry(html, name, modDate, size.str(), fileIcon);
	}
	addFooter(html);
	closedir(dir);
	return html.str();
}


void responseCreate(int client_socket, std::string content, std::string contentType, int status_code = 200)
{
	std::ostringstream response, log;
	response << "HTTP/1.1 " << status_code << getStatusMessage(status_code);
	response << "Server: WebServ/1.0\r\n";
	response << "Content-Type: " << contentType << "; charset=UTF-8\r\n";
	response << "Content-Length: " << content.size() << "\r\n";
	response << "\r\n";
	response << content;
	log << "Enviando resposta com código: " << status_code << ", Tamanho do conteúdo: " << content.size();
	getLogger().log(log.str());
	if (contentType != "image/jpeg" && contentType != "image/png" && contentType != "image/gif" && contentType != "image/x-icon" && contentType != "text/css")
		getLogger().log("\n\n\n" + response.str() + "\n\n");
	else
		getLogger().log("Imagem/css enviada");
	send(client_socket, response.str().c_str(), response.str().size(), 0);
	close(client_socket);
	getLogger().log("Conexão fechada");
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
	if (path.find(".ico") != std::string::npos)
		return "image/x-icon";
	return "text/plain";
}

void pathToFullPath(std::string &path)
{
	if (path == "/")
		path = "/index.html";
	path = "static" + path;
}

void handleGetRequest(int client_socket, std::string &path)
{
	pathToFullPath(path);
	if (isDirectory(path))
	{
		responseCreate(client_socket, listDirectory(path), "text/html");
		getLogger().log("Diretório enviado com sucesso: " + path);
		return;
	}
	std::string content = readFile(path);
	if (content.empty())
	{
		getLogger().log("Arquivo não encontrado: " + path);
		responseCreate(client_socket, getError404().getBody(), "text/html", 404);
	}
	else
	{
		responseCreate(client_socket, content, getContentType(path));
		getLogger().log("Arquivo enviado com sucesso: " + path);
	}
}

void serverLoop(int sockfd)
{
	sockaddrIn cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (sockaddr *)&cli_addr, &clilen);
	if (newsockfd < 0)
		ft_error("Erro ao aceitar conexão", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	getLogger().log("Nova conexão aceita pelo ip: " + inetNtop(cli_addr.sin_addr.s_addr));
	char buffer[4096];
	int bytes = recv(newsockfd, buffer, 4096, 0);
	if (bytes < 0)
		ft_error("Erro ao receber dados", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	std::string request(buffer);
	std::istringstream request_stream(request);
	std::string method, path, protocol;
	request_stream >> method >> path >> protocol;
	std::ostringstream log;
	log << "Requisição recebida: " << method << " " << path << " " << protocol;
	getLogger().log(log.str());
	if (method == "GET")
		handleGetRequest(newsockfd, path);
	else
	{
		responseCreate(newsockfd, getError404().getBody(), "text/html", 404);
		getLogger().log("Método não suportado: " + method);
	}
}

int serverInit(std::string ip, int port)
{
	sockaddrIn serv_addr;
	serv_addr.sin_family = AF_INET;
	if (!inetPton(ip, serv_addr.sin_addr.s_addr))
		ft_error("Erro ao converter endereço IP", __FUNCTION__, __FILE__, __LINE__, std::invalid_argument("byte inválido"));
	serv_addr.sin_port = htons(port);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ft_error("Erro ao criar socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		ft_error("Erro ao definir opção de socket", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
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
		getLogger().log("Fechando servidor");
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
		arr.push_back(serverInit("10.11.8.1", 13000));
		if (epoll_fd == -1)
			ft_error("Erro ao criar epoll", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = arr[0];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, arr[0], &event) == -1)
			ft_error("Erro ao adicionar socket ao epoll", __FUNCTION__, __FILE__, __LINE__, std::runtime_error(strerror(errno)));
		while (1)
			for (int i = 0; i < (int)arr.size(); i++)
				serverLoop(arr[i]);
	}
	catch (std::runtime_error &e)
	{
		for (std::vector<int>::iterator it = arr.begin(); it != arr.end(); it++)
			close(*it);
		close(epoll_fd);
		std::string msg = e.what();
		if (msg != "bye bye")
		{
			std::cerr << e.what() << std::endl;
			return 1;
		};
		std::cerr << e.what() << std::endl;
		return 0;
	}
	catch (std::exception &e)
	{
		return 1;
	}
	return 0;
}