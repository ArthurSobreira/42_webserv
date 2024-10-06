#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Config.hpp"

bool inetPton(const std::string &ip_str)
{
	Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
	std::istringstream stream(ip_str);
	std::string segment;
	std::vector<int> bytes;
	int byte;
	std::ostringstream log;

	while (std::getline(stream, segment, '.'))
	{
		std::istringstream str(segment);
		str >> byte;

		if (byte < 0 || byte > 255 || str.fail())
		{
			log << "Invalid byte in IP address: " << segment;
			logger.logError(LOG_ERROR, log.str());
			return false;
		}
		bytes.push_back(byte);
	}

	if (bytes.size() != 4)
	{
		log << "IP address should have exactly 4 bytes, got " << bytes.size();
		logger.logError(LOG_ERROR, log.str());
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Successfully converted IP to binary format: " + ip_str);
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

void ft_error(const char *message, const char *function, const char *file, int line, const std::exception &e)
{
	std::ostringstream oss;
	oss << "Erro: " << message << " in function " << function << " at " << file << ":" << line << " Exception: " << e.what();
	oss << std::endl;
	std::cerr << oss.str();
	throw e;
}

std::string readFile(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return "";
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

std::string getContentType(const std::string &uri){
	std::string extension = uri.substr(uri.find_last_of(".") + 1);
	if (extension == "html" || extension == "htm")
		return "text/html";
	if (extension == "css")
		return "text/css";
	if (extension == "js")
		return "text/javascript";
	if (extension == "jpeg" || extension == "jpg")
		return "image/jpeg";
	if (extension == "png")
		return "image/png";
	if (extension == "gif")
		return "image/gif";
	if (extension == "bmp")
		return "image/bmp";
	if (extension == "ico")
		return "image/x-icon";
	if (extension == "svg")
		return "image/svg+xml";
	if (extension == "json")
		return "application/json";
	if (extension == "pdf")
		return "application/pdf";
	if (extension == "zip")
		return "application/zip";
	if (extension == "tar")
		return "application/x-tar";
	if (extension == "gz")
		return "application/gzip";
	if (extension == "rar")
		return "application/x-rar-compressed";
	if (extension == "tar")
		return "application/x-tar";
	if (extension == "tar.gz")
		return "application/x-gzip";
	if (extension == "tar.Z")
		return "application/x-compress";
	if (extension == "tar.bz2")
		return "application/x-bzip2";
	if (extension == "7z")
		return "application/x-7z-compressed";
	if (extension == "txt")
		return "text/plain";
	if (extension == "rtf")
		return "application/rtf";
	if (extension == "doc")
		return "application/msword";
	if (extension == "docx")
		return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	if (extension == "xls")
		return "application/vnd.ms-excel";
	if (extension == "xlsx")
		return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	if (extension == "ppt")
		return "application/vnd.ms-powerpoint";
	if (extension == "pptx")
		return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	if (extension == "csv")
		return "text/csv";
	if (extension == "xml")
		return "application/xml";
	if (extension == "mp3")
		return "audio/mpeg";
	return "plain/text";
}


Config *config = NULL;

void setConfig(Config &c)
{
	if (!config)
		config = &c;
}

Config &getConfig()
{
	return *config;
}

void signals(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM)
		throw std::runtime_error("bye bye");
}