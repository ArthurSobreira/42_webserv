#include "GetResponse.hpp"

/* Constructor Method */
GetResponse::GetResponse( std::string filePath )
	: Response(), _filePath(filePath), _uri(filePath) {};

/* Destructor Method */
GetResponse::~GetResponse( void ) {};

/* Public Methods */
void	GetResponse::prepareResponse( const LocationConfigs &location ) {
	status Status;

	_filePath = location.root + _filePath;
	if (isDirectory(_filePath) && _filePath[_filePath.size() - 1] != '/') {
		_filePath += "/";
	}
	if (_filePath[_filePath.size() - 1] == '/' && !location.autoindex) {
		_filePath += location.index;
	}
	if (stat(_filePath.c_str(), &Status) != 0) {
		std::cout << "debug 01" << std::endl;
		handleError("404", location.server->errorPages.at("404"), ERROR_NOT_FOUND);
		return;
	} else if (S_ISDIR(Status.st_mode)) {
		listDirectoryHandler( location );
		return;
	} else if (access(_filePath.c_str(), R_OK) != 0) {
		handleError("403", location.server->errorPages.at("403"), ERROR_FORBIDDEN);
		return;
	}
	handleFileResponse(_filePath);
}

void	GetResponse::listDirectoryHandler( const LocationConfigs &location ) {
	_addHeader(_uri);
	if (!_listDirectory(_filePath)) {
		handleError("500", location.server->errorPages.at("500"), ERROR_INTERNAL_SERVER);
		return;
	}
	_addFooter();
	handleFileResponse("");
}

/* Private Methods */
void	GetResponse::_addHeader( const std::string &title ) {
	std::ostringstream oss;
	oss << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n"
		<< "<html>\n<head>\n"
		<< "<title>Index of " << title << "</title>\n"
		<< "</head>\n<body>\n"
		<< "<h1>Index of " << title << "</h1>\n"
		<< "<table>\n"
		<< "<tr><th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"[ICO]\"></th>"
		<< "<th><a href=\"?C=N;O=D\">Name</a></th>"
		<< "<th><a href=\"?C=M;O=A\">Last modified</a></th>"
		<< "<th><a href=\"?C=S;O=A\">Size</a></th>"
		<< "<tr><th colspan=\"5\"><hr></th></tr>\n";
	_body = oss.str();
}

bool	GetResponse::_listDirectory( const std::string &dirPath ) {
	std::map<std::string, status> filesDetails;
	DIR *dir = opendir(dirPath.c_str());
	if (!dir) {
		return false;
	}
	std::vector<std::string> folders;
	std::vector<std::string> files;

	diretory *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == ".")
			continue;
		if (entry->d_type == DT_DIR)
			folders.push_back(name);
		else
			files.push_back(name);
		status fileStatus;
		stat((dirPath + "/" + name).c_str(), &fileStatus);
		filesDetails[name] = fileStatus;
	}
	std::sort(folders.begin(), folders.end());
	std::sort(files.begin(), files.end());
	closedir(dir);
	_addFileEntry(folders, files, filesDetails);
	return true;
}

void	GetResponse::_addFileEntry( std::vector<std::string> &folders, 
	std::vector<std::string> &files, std::map<std::string, status> &filesDetails ){
	std::ostringstream oss;
	for (std::vector<std::string>::iterator it = folders.begin(); 
		it != folders.end(); ++it) {
		std::string name = *it;
		std::string modDate = ctime(&filesDetails[name].st_mtime);
		modDate = modDate.substr(0, modDate.size() - 1);
		std::string size = "-";
		if (_uri == "/") {
			_uri = DEFAULT_EMPTY;
		}
		std::string ref = _uri + "/" + name;
		std::cout << GREEN << ref << RESET << std::endl;
		if (name == "..") {
			if (_uri.size() > 1)
				ref = _uri.substr(0, _uri.find_last_of('/') + 1);
			else
				ref = _uri;
		}
		oss << "<tr><td valign=\"top\"><img src=\"/icons/folder.gif\" alt=\"[   ]\"></td>"
			<< "<td><a href=\"" << ref << "\">" << name << "</a></td>"
			<< "<td align=\"right\">" << modDate << "</td>"
			<< "<td align=\"right\">" << size << "</td>"
			<< "<td>&nbsp;</td></tr>\n";
	}
	for (std::vector<std::string>::iterator it = files.begin(); 
		it != files.end(); ++it) {
		std::stringstream sizeConverter;
		std::string name = *it;
		std::string modDate = ctime(&filesDetails[name].st_mtime);
		modDate = modDate.substr(0, modDate.size() - 1);
		sizeConverter << filesDetails[name].st_size;
		std::string size = sizeConverter.str();
		oss << "<tr><td valign=\"top\"><img src=\"/icons/unknown.gif\" alt=\"[   ]\"></td>"
			<< "<td><a href=\"" << _uri + "/" + name << "\">" << name << "</a></td>"
			<< "<td align=\"right\">" << modDate << "</td>"
			<< "<td align=\"right\">" << size << "</td>"
			<< "<td>&nbsp;</td></tr>\n";
	}
	_body += oss.str();
}

void	GetResponse::_addFooter( void ) {
	std::ostringstream oss;
	oss << "<tr><th colspan=\"5\"><hr></th></tr>\n"
		<< "</table>\n</body></html>";
	_body += oss.str();
}
