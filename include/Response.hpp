#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "Includes.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "HttpMessage.hpp"

class Response : public HttpMessage
{
public:
    // Construtor
    Response();

    void setStatus(int code, const std::string &reason);
    void setHeader(const std::string &key, const std::string &value);
    void setBody(const std::string &bodyContent);

    int getStatusCode() const;
    std::string getReasonPhrase() const;
    std::string getHeader(const std::string &key) const;
    std::string getBody() const;

    // Geração da resposta completa
    std::string generateResponse() const;

    // Tratamento da resposta com base na requisição
    void processRequest(const std::string &raw_request, const ServerConfigs* respconfig, Logger &logger);

    // Métodos auxiliares para erros
    void handleError(int status_code, const std::string &error_page, const std::string &error_message, Logger &logger);
    void handleFileResponse(const std::string &path, Logger &logger);

private:
    int _status_code;
    std::string _reason_phrase;
    std::string _root;

    // Métodos internos auxiliares
    void setBodyWithContentType(const std::string &bodyContent, const std::string &path);
    void handlerValidRequest(Logger &logger);
	bool validateMethod();
};

#endif // RESPONSE_HPP_