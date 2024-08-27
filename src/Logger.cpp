#include "Logger.hpp"

// Construtor padrão e parametrizado
Logger::Logger(const std::string &filename)
    : _logfile(filename.c_str(), std::ios::out | std::ios::app), _filename(filename)
{
    if (!_logfile.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo de log!" << std::endl;
    }
}

// Construtor de cópia
Logger::Logger(Logger const &src)
    : _logfile(src._filename.c_str(), std::ios::out | std::ios::app), _filename(src._filename)
{
    if (!_logfile.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo de log!" << std::endl;
    }
}

// Destrutor
Logger::~Logger()
{
    if (_logfile.is_open())
    {
        _logfile.close();
    }
}

// Operador de atribuição
Logger &Logger::operator=(Logger const &rhs)
{
    if (this != &rhs)
    {
        if (_logfile.is_open())
        {
            _logfile.close();
        }
        _filename = rhs._filename;
        _logfile.open(_filename.c_str(), std::ios::out | std::ios::app);
        if (!_logfile.is_open())
        {
            std::cerr << "Erro ao abrir o arquivo de log!" << std::endl;
        }
    }
    return *this;
}

void Logger::log(const std::string &message)
{
    if (_logfile.is_open())
    {
        _logfile << _currentDateTime() << " - " << message << std::endl;
    }
}

std::string Logger::_currentDateTime() const
{
    std::time_t now = std::time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *std::localtime(&now);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
