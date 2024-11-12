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
        /* Constructor Method */
        DeleteResponse( const std::string &filePath, 
            ServerConfigs &server );

        /* Destructor Method */
        ~DeleteResponse( void );

        /* Public Methods */
        void prepareResponse( void );
};

#endif // DELETERESPONSE_HPP
