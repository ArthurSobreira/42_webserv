#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "EpollManager.hpp"
#include "Fds.hpp"
#include "Request.hpp"
#include "GetResponse.hpp"
#include "PostResponse.hpp"
#include "DeleteResponse.hpp"
#include "CGIResponse.hpp"

typedef struct clientData {
    std::string request;
    std::string response;
    bool connection;
    bool completeRequest;
    size_t contentLength;
    size_t bytesRead;

    clientData(const std::string &req, const std::string &res, bool conn, bool compReq, size_t contentLen, size_t bRead = 0)
        : request(req), response(res), connection(conn), completeRequest(compReq), contentLength(contentLen), bytesRead(bRead) {}
    clientData() : request(""), response(""), connection(false), completeRequest(false), contentLength(0), bytesRead(0) {}
} clientData;

class ServerManager
{
public:
    ServerManager(const std::string &configFilePath);
    ~ServerManager();
    void run();

private:
    bool initializeServers();
    void handleEvents();
    void handleRead(int clientSocket);
    void handleWrite(int clientSocket);
    void acceptConnection(int serverSocket);
    void handleResponse(Request &request, ServerConfigs &server, int clientSocket);
    void handleError(int clientSocket, Logger *logger, const std::string &errorPage, const std::string &status);
    void closeConnection(int clientSocket);
    bool verifyContentLength(int clientSocket, std::string &buffer);
    void getContentLength(int clientSocket, std::string &buffer);

private:
    Logger *_logger;
    EpollManager _epollManager;
    Config _config;
    Fds _fds;
    std::vector<Server*> _servers;
    std::map<int, clientData> _clientDataMap;
    std::map<int, int> _clientServerMap;
};

#endif // SERVERMANAGER_HPP