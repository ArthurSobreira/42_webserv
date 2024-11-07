#ifndef DELETERESPONSE_HPP
#define DELETERESPONSE_HPP

#include "Includes.hpp"
#include "Defines.hpp"
#include "Response.hpp"

class DeleteResponse : public Response {
    private:
        std::string _filePath;
		ServerConfigs _server;
    public:
        DeleteResponse(const std::string &filePath, 
            ServerConfigs &server);

        void prepareResponse();
};

#endif // DELETERESPONSE_HPP
