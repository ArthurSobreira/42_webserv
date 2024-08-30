#include "includes.hpp"

bool inetPton(const std::string &ip_str, uint32_t &out_binary_ip)
{
	std::istringstream stream(ip_str);
	std::string segment;
	std::vector<int> bytes;
	int byte;

	while (std::getline(stream, segment, '.'))
	{

			std::istringstream str(segment);
			str >> byte;
			if (byte < 0 || byte > 255 || str.fail())
				return getLogger().log("Erro na conversão: byte inválido"), false;
			bytes.push_back(byte);
	}
	if (bytes.size() != 4)
		return false;
	out_binary_ip = htonl((bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3]);
	return true;
}


std::string inetNtop(uint32_t binary_ip)
{
    uint32_t host_ip = ntohl(binary_ip);

    int byte1 = (host_ip >> 24) & 0xFF;
    int byte2 = (host_ip >> 16) & 0xFF;
    int byte3 = (host_ip >> 8) & 0xFF;
    int byte4 = host_ip & 0xFF;
    std::ostringstream ip_stream;
    ip_stream << byte1 << "." << byte2 << "." << byte3 << "." << byte4;
    return ip_stream.str();
}

void ft_error(const char *message, const char *function, const char *file, int line, const std::exception& e)
{
    std::ostringstream  oss;
    oss << "Erro: " << message << " in function " << function << " at " << file << ":" << line << " Exception: " << e.what();
    getLogger().log(oss.str());
    oss << std::endl;
    std::cerr << oss.str();
    throw e; 
}
