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
	{
		std::cerr << "Endereço IP inválido." << std::endl;
		return false;
	}
	out_binary_ip = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
	return true;
}
