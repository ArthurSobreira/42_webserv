#ifndef UTILS_HPP
#define UTILS_HPP

class Logger;
typedef struct sockaddr_in sockaddrIn;

bool inetPton(const std::string &ip_str, uint32_t &out_binary_ip, Logger &logger);
void ft_error(const char *message, const char *function, const char *file, int line, const std::exception &e);
std::string inetNtop(uint32_t binary_ip);
std::string readFile(const std::string &path);

// ###################################//

#endif // UTILS_HPP
