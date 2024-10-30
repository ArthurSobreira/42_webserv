#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include "Response.hpp"
#include <string>

class CgiResponse : public Response {
    private:
        std::string _cgiPath;
        std::string _cgiOutput;

    public:
        CgiResponse(int clientSocket, Logger &logger, const std::string &cgiPath);

        void prepareResponse();
        void sendResponse();
};

#endif // CGIRESPONSE_HPP
