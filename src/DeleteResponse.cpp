#include "DeleteResponse.hpp"

DeleteResponse::DeleteResponse(const std::string &filePath, ServerConfigs &server) 
	: Response(), _filePath(filePath), _server(server) {}

void DeleteResponse::prepareResponse()
{
	// Verifica se o arquivo existe e se temos permissão para removê-lo
	if (access(_filePath.c_str(), F_OK) != 0)
	{
		handleError("404", _server.errorPages.at("404"), "Not Found", _logger);
		return;
	}

	if (access(_filePath.c_str(), W_OK) != 0)
	{
		handleError("403", _server.errorPages.at("403"), "Forbidden: No write permission", _logger);
		return;
	}
	// Tenta remover o arquivo
	if (remove(_filePath.c_str()) == 0)
	{
		_statusCode = "204";
		_reasonPhrase = "No Content";
	}
	else
	{
		handleError("500", _server.errorPages.at("500"), "Internal Server Error", _logger);
	}
}