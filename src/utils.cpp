#include "../include/includes.hpp"

bool inetPton(const std::string &ip_str, uint32_t &out_binary_ip)
{
	std::istringstream stream(ip_str);
	std::string segment;
	std::vector<int> bytes;

	while (std::getline(stream, segment, '.'))
	{
		try
		{
			int byte = std::stoi(segment);
			if (byte < 0 || byte > 255)
				throw std::out_of_range("Byte fora do intervalo.");
			bytes.push_back(byte);
		}
		catch (std::exception &e)
		{
			std::cerr << "Erro na conversão: " << e.what() << std::endl;
			return false;
		}
	}
	if (bytes.size() != 4)
		return false;
	out_binary_ip = htonl((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]);
	return true;
}

void ft_error(const char *message, const char *error_detail, const char *function, const char *file, int line, int error_code)
{
	std::cerr << "Erro: " << message << " (" << error_detail << ")"
			  << " in function " << function << " at " << file << ":" << line
			  << " Error code: " << error_code << std::endl;
}
