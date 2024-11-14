#include "Includes.hpp"
#include "Defines.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "Utils.hpp"

bool inetPton(const std::string &ip_str)
{
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

std::string readFile(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
		return "";
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

std::string getContentType(const std::string &uri)
{
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

std::string removeLastSlashes(const std::string &uri)
{
	std::string formatUri = uri;
	size_t queryPos = formatUri.find("?");
	if (queryPos != std::string::npos)
	{
		formatUri = formatUri.substr(0, queryPos);
	}

	while (!formatUri.empty() && formatUri[formatUri.length() - 1] == '/')
	{
		formatUri = formatUri.substr(0, formatUri.length() - 1);
	}
	return (formatUri);
}

std::string	getStringMethod(httpMethod method)
{
	if (method == GET) { return "GET"; }
	else if (method == POST) { return "POST"; }
	else if (method == DELETE) { return "DELETE"; }
	else { return DEFAULT_EMPTY; }
}

std::string getErrorMessage(const std::string &status)
{
	if (status == "400") { return ERROR_BAD_REQUEST; }
	if (status == "403") { return ERROR_FORBIDDEN; }
	if (status == "404") { return ERROR_NOT_FOUND; }
	if (status == "405") { return ERROR_METHOD_NOT_ALLOWED; }
	if (status == "413") { return ERROR_TOO_LARGE; }
	if (status == "415") { return ERROR_UNSUPPORTED_MEDIA_TYPE; }
	else { return ERROR_INTERNAL_SERVER; }
}

bool	counterOneSlash(const std::string &uri)
{
	int counter = 0;
	for (size_t i = 0; i < uri.size(); i++) {
		if (uri[i] == '/') {
			counter++;
		}
	}
	if (counter == 1 && uri[0] == '/') {
		return true;
	}
	return false;
}

bool createSocket(int &sockfd, int domain, int type)
{
	sockfd = socket(domain, type, 0);
	if (sockfd < 0)
	{
		logger.logError(LOG_ERROR, "Error opening socket");
		return false;
	}
	logger.logDebug(LOG_DEBUG, "Socket created");
	return true;
}

bool isDirectory(const std::string &path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

std::string	intToString(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

void exitHandler(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM)
	{
		stop = 1;
		logger.logDebug(LOG_INFO, "Exiting WebServ...👋🚪", true);
	}
}

void setupSignalHandlers(void)
{
	signal(SIGINT, exitHandler);
	signal(SIGQUIT, exitHandler);
}
