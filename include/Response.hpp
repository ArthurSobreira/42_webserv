
#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "Includes.hpp"
#include "Request.hpp"
#include "Logger.hpp"

class Response {
public:
    Response();


    // setters

    void setStatus(int code, const std::string &reason) ;
    void setHeader(const std::string &key, const std::string &value);
    void setBody(const std::string &bodyContent);
    // getters
    int getStatusCode() const;
    std::string getReasonPhrase();
    std::string getHeader(const std::string &key) const;
    std::string getBody() const;

    std::string generateResponse() const ;
    void responseTratament(Request &request, Logger &logger);


private:
    int status_code;
    std::string reason_phrase;
    std::map<std::string, std::string> headers;
    std::string body;
};

#endif // RESPONSE_HPP_
