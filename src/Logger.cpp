#include "Includes.hpp"
#include "Defines.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

Logger::Logger( const std::string &debugLog, const std::string &logAccess, 
    const std::string &logError )
    : _debugLog(debugLog.c_str(), std::ios::out | std::ios::app), 
      _logAccess(logAccess.c_str(), std::ios::out | std::ios::app), 
      _logError(logError.c_str(), std::ios::out | std::ios::app)
{
    if (!_debugLog.is_open() || !_logAccess.is_open() || !_logError.is_open())
        ft_error(ERROR_OPEN_LOG_FILE, __FUNCTION__, __FILE__, __LINE__, 
            std::runtime_error(ERROR_OPEN_LOG_FILE));
    this->logDebug("Starting debug log...", true);
    this->logAccess("Starting access log...", true);
    this->logError("INFO", "Starting error log...", true);
}

Logger::~Logger( void ) {
    if (_debugLog.is_open())
        _debugLog.close();
    if (_logAccess.is_open())
        _logAccess.close();
    if (_logError.is_open())
        _logError.close();
}

void Logger::logDebug( const std::string &message, bool tty ) {
    if (_debugLog.is_open())
        _debugLog << _currentDateTime() << " - " 
                    << message << std::endl;
    if (tty && _isTerminal(std::cout))
        std::cout << _currentDateTime() << " - " 
                    << message << std::endl;
}

void Logger::logAccess( const std::string &message, bool tty ) {
    if (_logAccess.is_open())
        _logAccess << _currentDateTime() << " - "
                   << message << std::endl;
    if (tty && _isTerminal(std::cout))
        std::cout << _currentDateTime() << " - "
                  << message << std::endl;
}

void Logger::logError( const std::string &severity,
            const std::string &message, bool tty )
{
    if (_logError.is_open())
        _logError << _currentDateTime() << " - "
                  << "[" << severity << "] "
                  << message << std::endl;
    if (tty && _isTerminal(std::cerr))
        std::cerr << _currentDateTime() << " - "
                  << "[" << severity << "] "
                  << message << std::endl;
}

std::string Logger::_currentDateTime( void ) const {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

bool Logger::_isTerminal( std::ostream &os ) const {
    if (&os == &std::cout || &os == &std::cerr)
        return (isatty(fileno(stdout)));
    return false;
}
