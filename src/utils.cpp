#include "../include/includes.hpp"

bool inetPton(const std::string &ip_str, uint32_t &out_binary_ip)
{
	std::istringstream stream(ip_str);
	std::string segment;
	std::vector<int> bytes;
	int byte;

	while (std::getline(stream, segment, '.'))
	{
		try
		{
			std::istringstream str(segment);
			str >> byte;
			if (byte < 0 || byte > 255 || str.fail())
			{
				Logger::log("Erro na conversão: byte inválido");
			}
			bytes.push_back(byte);
		}
		catch (std::exception &e)
		{
			std::cerr << "Erro na conversão: " << e.what() << std::endl;
			return std::cout << "Erro na conversão: " << e.what() << std::endl, false;
		}
	}
	if (bytes.size() != 4)
		return false;
	out_binary_ip = htonl((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]);
	return true;
}

void ft_error(const char *message, const char *function, const char *file, int line, const std::exception& e)
{
    std::ostringstream  oss;
    Logger log("logs/server.log");

    oss << "Erro: " << message << " in function " << function << " at " << file << ":" << line << " Error code: " << " Exception: " << e.what();
    log.log(oss.str());
    oss << std::endl;
    std::cerr << oss.str();
    throw e; 
}
