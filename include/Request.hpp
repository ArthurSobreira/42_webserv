#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "HttpMessage.hpp"
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <iostream>

class Request : public HttpMessage
{
	private:
		std::string method;
		std::string uri;
		bool requestIsValid;
		std::string rawRequest;

	public:
		Request();

		// Função para processar e validar a requisição
		bool parseRequest(const std::string& raw_request);
		bool isComplete(const std::string& raw_request) const;
		bool keepAlive() const;

		// Getters
		std::string getMethod() const;
		std::string getUri() const;
		bool isRequestValid() const;
		std::string getRawRequest() const;

		// Setters (se necessário)
		void setMethod(const std::string& m);
		void setUri(const std::string& u);
		void setRawRequest(const std::string& r);

		// Funções auxiliares
		bool validateContentLength();


	private:
		bool parseStartLine(const std::string& start_line);
		bool parseHeaders(const std::string& header_part);
		bool validateMethod();
		bool validateHttpVersion();
		std::set<std::string> createValidMethods();

};

#endif // REQUEST_HPP
