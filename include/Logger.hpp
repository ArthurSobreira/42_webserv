#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

class Logger
{
public:
    Logger(const std::string &log, const std::string &logAccess, const std::string &logError);
    ~Logger();
    void logDebug(const std::string &message);
    void logAccess(const std::string& client_ip, const std::string& request_line, int status_code, size_t response_size);
    void logError(const std::string &severity, const std::string &message);

private:
    std::ofstream _debugLog;
    std::ofstream _logAccess;
    std::ofstream _logError;


    std::string _currentDateTime() const;
};

#endif // LOGGER_HPP
