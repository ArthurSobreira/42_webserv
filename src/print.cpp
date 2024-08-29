// std::string listDirectory(const std::string& dirPath) {
// 	DIR* dir = opendir(dirPath.c_str());
// 	if (!dir) {
// 		return "<p>Erro ao abrir o diretório.</p>";
// 	}

// 	std::ostringstream html;

// 	struct dirent* entry;
// 	html << "<html><head><title>Index of " << dirPath << "</title></head><body>";
// 	html << "<h1>Index of " << dirPath << "</h1>";
// 	html << "<ul>";
// 	while ((entry = readdir(dir)) != NULL) {
// 		html << "<li><a href=\"" << dirPath << "/" << entry->d_name << "\">" << entry->d_name << "</a></li>";
// 	}
// 	html << "</ul>";
// 	html << "</body></html>";

// 	closedir(dir);
// 	return html.str();
// }


// bool isDirectory(const std::string& path) {
// 	struct stat statbuf;
// 	if (stat(path.c_str(), &statbuf) != 0) 
// 		return false;
// 	return S_ISDIR(statbuf.st_mode);
// }