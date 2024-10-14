#include "Server.hpp"
#include "Logger.hpp"
#include "Includes.hpp"
#include "Defines.hpp"
#include "Request.hpp"

void testCreateSocket()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int sockfd;

    std::cout << COLORIZE(YELLOW, "Test 1: Creating a valid socket...") << std::endl;
    if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger))
        std::cout << COLORIZE(GREEN, "Test 1 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 1 failed!") << std::endl;

    std::cout << COLORIZE(YELLOW, "Test 2: Creating an invalid socket (invalid domain)...") << std::endl;
    if (!createSocket(sockfd, -1, SOCK_STREAM, logger))
        std::cout << COLORIZE(GREEN, "Test 2 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 2 failed!") << std::endl;

    closeSocket(sockfd, logger); // Fechar o socket no final do teste.
}

void testBindSocket()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int sockfd;
    sockaddr_in serv_addr;

    std::cout << COLORIZE(YELLOW, "Test 6: Binding valid socket to port 8080...") << std::endl;
    if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger) &&
        bindSocket(sockfd, 8080, INADDR_ANY, serv_addr, logger)) // Corrigido aqui
        std::cout << COLORIZE(GREEN, "Test 6 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 6 failed!") << std::endl;

    closeSocket(sockfd, logger);
}

void testListenSocket()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int sockfd;
    int backlog = 5;
    sockaddr_in serv_addr;

    std::cout << COLORIZE(YELLOW, "Test 7: Listening on valid socket...") << std::endl;
    if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger) &&
        configureSocket(sockfd, logger) &&
        bindSocket(sockfd, 8080, INADDR_ANY, serv_addr, logger) && // Corrigido aqui
        listenSocket(sockfd, backlog, logger)) // Corrigido aqui
        std::cout << COLORIZE(GREEN, "Test 7 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 7 failed!") << std::endl;

    closeSocket(sockfd, logger);
}

void testCreateServer()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int sockfd;
    int backlog = 5;

    std::cout << COLORIZE(YELLOW, "Test 8: Creating a valid server...") << std::endl;
    if (createServer(sockfd, 8080, backlog, logger))
        std::cout << COLORIZE(GREEN, "Test 8 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 8 failed!") << std::endl;

    closeSocket(sockfd, logger);
}

void testLogErrorAndClose()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int sockfd;

    sockfd = open(LOG_FILE, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    std::cout << COLORIZE(YELLOW, "Test 9: Logging an error and closing a socket...") << std::endl;
    if (!logErrorAndClose("Error message", sockfd, logger))
        std::cout << COLORIZE(GREEN, "Test 9 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 9 failed!") << std::endl;
}

void testValidateBacklog()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int backlog = 5;

    std::cout << COLORIZE(YELLOW, "Test 10: Validating a valid backlog...") << std::endl;
    if (validateBacklog(backlog, logger))
        std::cout << COLORIZE(GREEN, "Test 10 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 10 failed!") << std::endl;

    backlog = -1;
    std::cout << COLORIZE(YELLOW, "Test 11: Validating an invalid backlog...") << std::endl;
    if (!validateBacklog(backlog, logger))
        std::cout << COLORIZE(GREEN, "Test 11 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 11 failed!") << std::endl;
}

void testConfigureSocket()
{
    Logger logger(LOG_FILE, LOG_ACCESS_FILE, LOG_ERROR_FILE);
    int sockfd;

    std::cout << COLORIZE(YELLOW, "Test 12: Configuring a valid socket...") << std::endl;
    if (createSocket(sockfd, AF_INET, SOCK_STREAM, logger) &&
        configureSocket(sockfd, logger))
        std::cout << COLORIZE(GREEN, "Test 12 passed!") << std::endl;
    else
        std::cout << COLORIZE(RED, "Test 12 failed!") << std::endl;

    closeSocket(sockfd, logger);
}

void testParseValidRequest() {
    Request request;
    std::string raw_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: test\r\n"
        "Content-Length: 0\r\n\r\n";

    std::cout << COLORIZE(YELLOW, "Test 13: Parsing a valid HTTP request...") << std::endl;
    if (request.parseRequest(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 13 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 13 failed!") << std::endl;
    }
}

void testParseInvalidRequest() {
    Request request;
    std::string raw_request = 
        "INVALID /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n";

    std::cout << COLORIZE(YELLOW, "Test 14: Parsing an invalid HTTP request (invalid method)...") << std::endl;
    if (!request.parseRequest(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 14 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 14 failed!") << std::endl;
    }
}

void testIncompleteRequest() {
    Request request;
    std::string raw_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n";

    std::cout << COLORIZE(YELLOW, "Test 15: Checking incomplete request (missing headers end)...") << std::endl;
    if (!request.isComplete(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 15 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 15 failed!") << std::endl;
    }
}

void testGetMethod() {
    Request request;
    std::string raw_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n";
    request.parseRequest(raw_request);

    std::cout << COLORIZE(YELLOW, "Test 16: Checking request method is GET...") << std::endl;
    if (request.getMethod() == "GET") {
        std::cout << COLORIZE(GREEN, "Test 16 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 16 failed!") << std::endl;
    }
}

void testGetUri() {
    Request request;
    std::string raw_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n";
    request.parseRequest(raw_request);

    std::cout << COLORIZE(YELLOW, "Test 17: Checking request URI is /index.html...") << std::endl;
    if (request.getUri() == "/index.html") {
        std::cout << COLORIZE(GREEN, "Test 17 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 17 failed!") << std::endl;
    }
}

void testParsePostRequest() {
    Request request;
    std::string raw_request = 
        "POST /submit-form HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 27\r\n\r\n"
        "field1=value1&field2=value2";

    std::cout << COLORIZE(YELLOW, "Test 18: Parsing a valid POST request...") << std::endl;
    if (request.parseRequest(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 18 passed!") << std::endl;

        if (request.getMethod() == "POST") {
            std::cout << COLORIZE(GREEN, "Method is POST: Test 18.1 passed!") << std::endl;
        } else {
            std::cout << COLORIZE(RED, "Method is not POST: Test 18.1 failed!") << std::endl;
        }

        if (request.getBody() == "field1=value1&field2=value2") {
            std::cout << COLORIZE(GREEN, "Body is correct: Test 18.2 passed!") << std::endl;
        } else {
            std::cout << COLORIZE(RED, "Body is incorrect: Test 18.2 failed!") << std::endl;
        }

    } else {
        std::cout << COLORIZE(RED, "Test 18 failed!") << std::endl;
    }
}

void testValidateContentLength() {
	Request request;
	std::string raw_request = 
		"POST /submit-form HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 27\r\n\r\n"
		"field1=value1&field2=value2";

	std::cout << COLORIZE(YELLOW, "Test 19: Validating Content-Length header...") << std::endl;
	if (request.parseRequest(raw_request) && request.validateContentLength()) {
		std::cout << COLORIZE(GREEN, "Test 19 passed!") << std::endl;
	} else {
		std::cout << COLORIZE(RED, "Test 19 failed!") << std::endl;
	}
}

void testInvalidContentLength() {
	Request request;
	std::string raw_request = 
		"POST /submit-form HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 28\r\n\r\n"
		"field1=value1&field2=value2";

	std::cout << COLORIZE(YELLOW, "Test 20: Invalid Content-Length header...") << std::endl;
	if (!request.parseRequest(raw_request) && !request.validateContentLength()) {
		std::cout << COLORIZE(GREEN, "Test 20 passed!") << std::endl;
	} else {
		std::cout << COLORIZE(RED, "Test 20 failed!") << std::endl;
	}
}

void testIsCompleteRequest() {
    Request request;
    std::string raw_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 13\r\n\r\n"
        "Hello, world!";

    std::cout << COLORIZE(YELLOW, "Test 21: Checking if request is complete...") << std::endl;
    if (request.isComplete(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 21 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 21 failed!") << std::endl;
    }
}

void testIsCompleteChunkedRequest() {
    Request request;
    std::string raw_request = 
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Transfer-Encoding: chunked\r\n\r\n"
        "4\r\nWiki\r\n"
        "5\r\npedia\r\n"
        "E\r\n in\r\n\r\nchunks.\r\n"
        "0\r\n\r\n";

    std::cout << COLORIZE(YELLOW, "Test 22: Checking if chunked request is complete...") << std::endl;
    if (request.isComplete(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 22 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 22 failed!") << std::endl;
    }
}

void testIsCompleteGetRequest() {
    Request request;
    std::string raw_request = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n";

    std::cout << COLORIZE(YELLOW, "Test 23: Checking if GET request is complete...") << std::endl;
    if (request.isComplete(raw_request)) {
        std::cout << COLORIZE(GREEN, "Test 23 passed!") << std::endl;
    } else {
        std::cout << COLORIZE(RED, "Test 23 failed!") << std::endl;
    }
}

int main()
{
    // Teste individual de cada função
    testCreateSocket();
    testBindSocket();
    testListenSocket();
    testCreateServer();
    testLogErrorAndClose();
    testValidateBacklog();
    testConfigureSocket();

    // Testes da classe Request
    testParseValidRequest();
    testParseInvalidRequest();
    testIncompleteRequest();
    testGetMethod();
    testGetUri();
    testIsCompleteRequest(); // Adicionando o novo teste aqui
	testIsCompleteChunkedRequest(); // Adicionando o novo teste aqui
    testIsCompleteGetRequest(); // Adicionando o novo teste aqui



    // Testes de requisição POST
    testParsePostRequest();
	testValidateContentLength();
	testInvalidContentLength();

    return 0;
}