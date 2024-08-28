#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>

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
    return "text/plain";
}

// Função para criar e enviar a resposta HTTP
void responseCreate(int client_socket, std::string content, std::string contentType, int status_code = 200)
{
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << " OK\r\n";
    response << "Server: WebServ/1.0\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "\r\n";
    response << content;
    send(client_socket, response.str().c_str(), response.str().size(), 0);
    close(client_socket);
}

// Função para ler e retornar o conteúdo de um arquivo
std::string readFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
        return "";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Função para verificar se o caminho é um diretório
bool isDirectory(const std::string &path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

int main(void)
{
    std::string ip = "10.11.9.1";
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Erro ao criar socket");
        return 1;
    }
    std::cout << "Socket criado com sucesso!" << std::endl;

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr.s_addr);
    server_address.sin_port = htons(13000);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Erro ao fazer bind");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) < 0)
    {
        perror("Erro ao escutar");
        close(server_socket);
        return 1;
    }
    std::cout << "Servidor escutando na porta 13000..." << std::endl;

    while (true)
    {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0)
        {
            perror("Erro ao aceitar conexão");
            continue;
        }
        std::cout << "Cliente conectado!" << std::endl;

        char buffer[4096];
        recv(client_socket, buffer, 4096, 0);

        std::string request(buffer);
        std::istringstream request_stream(request);
        std::string method, path;
        request_stream >> method >> path;

        std::cout << "Requisição: " << method << " PATH: " << path << std::endl;

        if (path == "/")
        {
            path = "/index.html";
        }

        std::string fullPath = "static" + path;

        // Verifica se o caminho é um diretório e tenta servir index.html
        if (isDirectory(fullPath))
        {
            fullPath += "/index.html";
        }

        std::string content = readFile(fullPath);
        if (content.empty())
        {
            std::string notFound = "<h1>404 Not Found</h1>";
            responseCreate(client_socket, notFound, "text/html", 404);
        }
        else
        {
            std::string contentType = getContentType(fullPath);
            responseCreate(client_socket, content, contentType);
        }
    }

    close(server_socket);
    return 0;
}
