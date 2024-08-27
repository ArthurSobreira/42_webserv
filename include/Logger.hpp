#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <string>
# include <fstream>
# include <iostream>
# include <ctime>

class Logger
{
public:
    Logger(const std::string &filename);
    Logger(Logger const &src);
    ~Logger();
    Logger &operator=(Logger const &rhs);
    void log(const std::string &message);

private:
    std::ofstream _logfile;
	std::string _filename;

    std::string _currentDateTime() const;
};

#endif // LOGGER_HPP