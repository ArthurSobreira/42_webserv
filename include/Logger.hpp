#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Includes.hpp"

class Logger {
    private:
        std::ofstream _debugLog;
        std::ofstream _logAccess;
        std::ofstream _logError;

    public:
        /* Constructor Method */
        Logger( const std::string &log, const std::string &logAccess, 
            const std::string &logError );

        /* Destructor Method */
        ~Logger( void );

        /* Public Methods */
        std::string _currentDateTime( void ) const;
        void logDebug( const std::string &message );
        void logAccess( const std::string &message );
        void logError( const std::string &severity,
            const std::string &message, bool tty = false );
};

#endif
