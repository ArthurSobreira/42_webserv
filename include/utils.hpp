#ifndef UTILS_HPP
#define UTILS_HPP

bool inetPton(const std::string &ip_str, uint32_t &out_binary_ip);
void ft_error(const char *message, const char *function, const char *file, int line, const std::exception& e);


#endif // UTILS_HPP