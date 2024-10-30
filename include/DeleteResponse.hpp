#ifndef DELETERESPONSE_HPP
#define DELETERESPONSE_HPP

#include "Response.hpp"
#include <string>

class DeleteResponse : public Response {
    private:
        std::string _filePath;
        std::string _responseData;

    public:
        DeleteResponse(int clientSocket, Logger &logger, const std::string &filePath);

        void prepareResponse();
        void sendResponse();
};

#endif // DELETERESPONSE_HPP
