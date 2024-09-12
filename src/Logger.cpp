#include "includes.hpp"

Logger::Logger(const std::string &debugLog, const std::string &logAccess, const std::string &logError)
    : _debugLog(debugLog.c_str(), std::ios::out | std::ios::app), 
      _logAccess(logAccess.c_str(), std::ios::out | std::ios::app), 
      _logError(logError.c_str(), std::ios::out | std::ios::app)
{
    if (!_debugLog.is_open() || !_logAccess.is_open() || !_logError.is_open())
        ft_error("Erro ao abrir o arquivo de log!", __FUNCTION__, __FILE__, __LINE__, std::runtime_error("Erro ao abrir o arquivo de log!"));
    if (_debugLog.is_open())
        _debugLog << "Iniciando log..." << std::endl;
    if (_logAccess.is_open())
        _logAccess << "Iniciando log..." << std::endl;
    if (_logError.is_open())
        _logError << "Iniciando log..." << std::endl;
}
Logger::~Logger()
{
    if (_debugLog.is_open())
        _debugLog.close();
    if (_logAccess.is_open())
        _logAccess.close();
    if (_logError.is_open())
        _logError.close();
}


void Logger::logDebug(const std::string &message)
{
    if (_debugLog.is_open())
        _debugLog << _currentDateTime() << " - " << message << std::endl;
}

void Logger::logAccess(const std::string &message)
{
    if (_logAccess.is_open())
        _logAccess << _currentDateTime() << " "
                   << message << std::endl;
}

void Logger::logError(const std::string &severity, const std::string &message)
{
    if (_logError.is_open())
        _logError << _currentDateTime() << " "
                  << "[" << severity << "] "
                  << message << std::endl;
}

std::string Logger::_currentDateTime() const
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
