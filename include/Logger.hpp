#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Includes.hpp"

class Logger
{
public:
    Logger(const std::string &log, const std::string &logAccess, const std::string &logError);
    ~Logger();
    void logDebug(const std::string &message);
    void logAccess(const std::string &message);
    void logError(const std::string &severity, const std::string &message);

private:
    std::ofstream _debugLog;
    std::ofstream _logAccess;
    std::ofstream _logError;


    std::string _currentDateTime() const;
};

#endif // LOGGER_HPP
