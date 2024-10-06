#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "Includes.hpp"
#include "Request.hpp"
#include "Logger.hpp"

class Response {
public:
    // Construtor
    Response();

    // Métodos para configurar a resposta
    void setStatus(int code, const std::string &reason);
    void setHeader(const std::string &key, const std::string &value);
    void setBody(const std::string &bodyContent);

    // Métodos para acessar a resposta
    int getStatusCode() const;
    std::string getReasonPhrase() const;
    std::string getHeader(const std::string &key) const;
    std::string getBody() const;

    // Geração da resposta completa
    std::string generateResponse() const;

    // Tratamento da resposta com base na requisição
    void processRequest(Request &request, Logger &logger);

    // Métodos auxiliares para erros
    void handleError(int status_code, const std::string &error_page, const std::string &error_message, Logger &logger);
    void handleFileResponse(const std::string &path, Logger &logger);

private:
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;

    // Métodos internos auxiliares
    void setBodyWithContentType(const std::string &bodyContent, const std::string &path);
	void handlerValidRequest(Request &request, Logger &logger);
};

#endif // RESPONSE_HPP_
