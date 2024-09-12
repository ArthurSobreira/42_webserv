// #include "Logger.hpp"

// Logger::Logger(const std::string &log, const std::string &logAccess, const std::string &logError)
//     : _logfile(log.c_str(), std::ios::out | std::ios::app), _filename(log)
// {
//     if (!_logfile.is_open())
//         std::cerr << "Erro ao abrir o arquivo de log!" << std::endl;
// }

// Logger::Logger(Logger const &src)
//     : _logfile(src._filename.c_str(), std::ios::out | std::ios::app), _filename(src._filename)
// {
//     if (!_logfile.is_open())
//     {
//         std::cerr << "Erro ao abrir o arquivo de log!" << std::endl;
//     }
// }

// Logger::~Logger()
// {
//     if (_logfile.is_open())
//         _logfile.close();
// }

// Logger &Logger::operator=(Logger const &rhs)
// {
//     if (this != &rhs)
//     {
//         if (_logfile.is_open())
//             _logfile.close();
//         _filename = rhs._filename;
//         _logfile.open(_filename.c_str(), std::ios::out | std::ios::app);
//         if (!_logfile.is_open())
//             std::cerr << "Erro ao abrir o arquivo de log!" << std::endl;
//     }
//     return *this;
// }

// void Logger::log(const std::string &message)
// {
//     if (_logfile.is_open())
//         _logfile << _currentDateTime() << " - " << message << std::endl;
// }

// void Logger::logAccess(const std::string& client_ip, const std::string& request_line, int status_code, size_t response_size) {
//     _logfile << _currentDateTime() << " "
//              << client_ip << " "
//              << "\"" << request_line << "\" "
//              << status_code << " "
//              << response_size << std::endl;
// }

// void Logger::logError(const std::string& severity, const std::string& message) {
//     _logfile << _currentDateTime() << " "
//              << "[" << severity << "] "
//              << message << std::endl;
// }

// std::string Logger::_currentDateTime() const
// {
//     std::time_t now = std::time(0);
//     struct tm tstruct;
//     char buf[80];
//     tstruct = *std::localtime(&now);
//     std::strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

//     return buf;
// }
