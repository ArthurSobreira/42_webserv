#include "Request.hpp"
#include "Includes.hpp"

Request::Request() : HttpMessage(), method(""), uri(""), requestIsValid(false) {}

bool Request::parseRequest(const std::string &raw_request)
{
	rawRequest = raw_request;
	std::cout << "Raw request: " << raw_request << std::endl;

	size_t body_start = raw_request.find("\r\n\r\n");
	if (body_start == std::string::npos)
	{
		std::cout << "debbug 01" << std::endl;
		return false;
	}

	std::string header_part = raw_request.substr(0, body_start);
	body = raw_request.substr(body_start + 4);

	if (!parseStartLine(header_part))
	{
		std::cout << "debbug 02" << std::endl;
		return false;
	}

	if (!parseHeaders(header_part))
	{
		std::cout << "debbug 03" << std::endl;
		return false;
	}

	// if (!validateContentLength())
	// {
	// 	std::cout << "debbug 04" << std::endl;
	// 	return false;
	// }

	requestIsValid = true;
	return true;
}

int hexstr_to_int(const std::string &hexstr)
{
	int value = 0;
	for (std::string::const_iterator it = hexstr.begin(); it != hexstr.end(); ++it)
	{
		char c = *it;
		value *= 16;
		if (c >= '0' && c <= '9')
			value += c - '0';
		else if (c >= 'a' && c <= 'f')
			value += c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			value += c - 'A' + 10;
		else
			return -1;
	}
	return value;
}

std::string Request::getRawRequest() const
{
	return rawRequest;
}

void Request::setRawRequest(const std::string &r)
{
	rawRequest = r;
}

bool processChunkedBody(std::string &raw_request, std::string &body)
{
	size_t pos = 0;

	while (true)
	{
		// Encontrar o final do tamanho do chunk (linha em branco após o valor)
		size_t chunk_size_end = raw_request.find("\r\n", pos);
		if (chunk_size_end == std::string::npos)
			return false; // Cabeçalho do chunk incompleto
		// Extrair o tamanho do chunk em hexadecimal
		std::string chunk_size_hex = raw_request.substr(pos, chunk_size_end - pos);
		int chunk_size = hexstr_to_int(chunk_size_hex);
		pos = chunk_size_end + 2; // Avançar para o início dos dados
		// Se o tamanho do chunk é 0, terminamos
		if (chunk_size == 0)
			break;
		// Verificar se os dados completos do chunk estão presentes
		if (raw_request.size() < pos + chunk_size + 2)
			return false; // Ainda não recebemos todos os dados do chunk
		// Adicionar os dados do chunk ao corpo
		body.append(raw_request, pos, chunk_size);
		pos += chunk_size + 2; // Avançar para o próximo chunk (pulando o \r\n após o chunk)
	}
	return true; // Todos os chunks foram processados
}

bool Request::isComplete(const std::string &raw_request) const
{
	size_t header_end = raw_request.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return false; // Cabeçalhos incompletos

	std::string body_part = raw_request.substr(header_end + 4);
	std::string content_length_str = getHeader("Content-Length");

	if (!content_length_str.empty())
	{
		int content_length;
		std::stringstream content_length_stream(content_length_str);
		content_length_stream >> content_length;

		std::cout << "Content-Length: " << content_length << std::endl;
		std::cout << "Body size: " << body_part.size() << std::endl;

		// Verifica se a quantidade de dados recebidos corresponde ao Content-Length
		if (raw_request.size() >= header_end + 4 + content_length)
		{
			std::cout << COLORIZE("debug request 989", GREEN) << std::endl;
			return true;
		}
	}
	// Verifica se Transfer-Encoding é chunked
	else if (getHeader("Transfer-Encoding") == "chunked")
	{
		std::string temp_body;
		if (processChunkedBody(body_part, temp_body))
			return true;
	}

	return false; // A requisição ainda não está completa
}

// Getters
std::string Request::getMethod() const
{
	return method;
}

std::string Request::getUri() const
{
	return uri;
}

bool Request::isRequestValid() const
{
	return requestIsValid;
}

// Setters
void Request::setMethod(const std::string &m)
{
	method = m;
}

void Request::setUri(const std::string &u)
{
	uri = u;
}

// Função para parsear a linha inicial da requisição
bool Request::parseStartLine(const std::string &start_line)
{
	std::istringstream stream(start_line);
	stream >> method >> uri >> http_version;

	return validateMethod() && validateHttpVersion();
}

// Função para parsear e armazenar os headers
bool Request::parseHeaders(const std::string &header_part)
{
	std::istringstream stream(header_part);
	std::string line;
	while (std::getline(stream, line) && line != "\r\n")
	{
		size_t pos = line.find(": ");
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			headers[key] = value;
		}
	}
	return !headers.empty();
}

// Função para validar o Content-Length
bool Request::validateContentLength()
{
	if (headers.find("Content-Length") != headers.end())
	{
		std::istringstream content_length_stream(headers["Content-Length"]);
		size_t content_length;
		content_length_stream >> content_length;
		std::cout << "Content-Length: " << content_length << std::endl;
		std::cout << "Body size: " << body.size() << std::endl;
		return body.size() == content_length;
	}
	return true;
}

// Validações específicas da Request
bool Request::validateMethod()
{
	static const std::set<std::string> valid_methods = createValidMethods();
	return valid_methods.find(method) != valid_methods.end();
}

bool Request::validateHttpVersion()
{
	return (http_version == "HTTP/1.1" || http_version == "HTTP/1.0");
}

std::set<std::string> Request::createValidMethods()
{
	std::set<std::string> valid_methods;
	valid_methods.insert("GET");
	valid_methods.insert("POST");
	valid_methods.insert("DELETE");
	return valid_methods;
}

bool Request::keepAlive() const
{
	return headers.find("Connection") != headers.end() && headers.at("Connection") == "keep-alive";
}