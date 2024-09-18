// void addHeader(std::ostringstream &oss, const std::string &title) {
//	 oss << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n"
//		 << "<html>\n<head>\n"
//		 << "<title>Index of " << title << "</title>\n"
//		 << "</head>\n<body>\n"
//		 << "<h1>Index of " << title << "</h1>\n"
//		 << "<table>\n"
//		 << "<tr><th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"[ICO]\"></th>"
//		 << "<th><a href=\"?C=N;O=D\">Name</a></th>"
//		 << "<th><a href=\"?C=M;O=A\">Last modified</a></th>"
//		 << "<th><a href=\"?C=S;O=A\">Size</a></th>"
//		 << "<th><a href=\"?C=D;O=A\">Description</a></th></tr>\n"
//		 << "<tr><th colspan=\"5\"><hr></th></tr>\n";
// }

// void addFileEntry(std::ostringstream &oss, const std::string &name, const std::string &modDate, const std::string &size, std::string &icon) {
// 	if(name.find(".html") != std::string::npos)
// 		icon = "/icons/html.png";
// 	else if (name.find(".png") != std::string::npos || name.find(".jpg") != std::string::npos || name.find(".jpeg") != std::string::npos)
// 		icon = "/icons/image.png";
//	 oss << "<tr><td valign=\"top\"><img src=\"" << icon << "\" alt=\"[   ]\"></td>"
//		 << "<td><a href=\"" << name << "\">" << name << "</a></td>"
//		 << "<td align=\"right\">" << modDate << "</td>"
//		 << "<td align=\"right\">" << size << "</td>"
//		 << "<td>&nbsp;</td></tr>\n";
// }

// void addFooter(std::ostringstream &oss) {
//	 oss << "<tr><th colspan=\"5\"><hr></th></tr>\n"
//		 << "</table>\n</body></html>";
// }

// bool isDirectory(const std::string& path) {
// 	struct stat statbuf;
// 	if (stat(path.c_str(), &statbuf) != 0) 
// 		return false;
// 	return S_ISDIR(statbuf.st_mode);
// }

// std::string listDirectory(const std::string& dirPath) {
// 	std::string dirIcon = "/icons/folder.gif";
// 	std::string fileIcon = "/icons/unknown.png";
// 	DIR* dir = opendir(dirPath.c_str());
// 	if (!dir) {
// 		return "<p>Erro ao abrir o diretório.</p>";
// 	}

// 	std::ostringstream html;
// 	addHeader(html, dirPath);

// 	diretory *entry;
// 	while ((entry = readdir(dir)) != NULL) {
// 		std::string name = entry->d_name;
// 		if (name == ".") 
// 			continue;
// 		std::string path = dirPath + "/" + name;
// 		status statbuf;
// 		stat(path.c_str(), &statbuf);
// 		std::string modDate = ctime(&statbuf.st_mtime);
// 		std::ostringstream size; 
// 		size << statbuf.st_size;
// 		if (isDirectory(path))
// 			addFileEntry(html, name + "/", modDate, size.str(), dirIcon);
// 		else 
// 			addFileEntry(html, name, modDate, size.str(), fileIcon);
// 	}
// 	addFooter(html);
// 	closedir(dir);
// 	return html.str();
// }


// void responseCreate(int client_socket, std::string content, std::string contentType, int status_code = 200)
// {
// 	std::ostringstream response, log;
// 	response << "HTTP/1.1 " << status_code << getStatusMessage(status_code);
// 	response << "Server: WebServ/1.0\r\n";
// 	response << "Content-Type: " << contentType << "; charset=UTF-8\r\n";
// 	response << "Content-Length: " << content.size() << "\r\n";
// 	response << "\r\n";
// 	response << content;
// 	log << "Enviando resposta com código: " << status_code << ", Tamanho do conteúdo: " << content.size();
// 	getLogger().log(log.str());
// 	if (contentType != "image/jpeg" && contentType != "image/png" && contentType != "image/gif" && contentType != "image/x-icon" && contentType != "text/css")
// 		getLogger().log("\n\n\n" + response.str() + "\n\n");
// 	else
// 		getLogger().log("Imagem/css enviada");
// 	send(client_socket, response.str().c_str(), response.str().size(), 0);
// 	close(client_socket);
// 	getLogger().log("Conexão fechada");
// }

// void signals(int sig)
// {
// 	if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM)
// 	{
// 		getLogger().log("Sinal de interrupção recebido");
// 		getLogger().log("Fechando servidor");
// 		throw std::runtime_error("bye bye");
// 	}
// }