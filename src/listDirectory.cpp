#include "Includes.hpp"
#include "Utils.hpp"

void addHeader(std::ostringstream &oss, const std::string &title)
{
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
		<< "<th><a href=\"?C=D;O=A\">Description</a></th></tr>\n"
		<< "<tr><th colspan=\"5\"><hr></th></tr>\n";
}

void addFileEntry(std::ostringstream &oss, const std::string &dirPath, const std::string &name, const std::string &modDate, const std::string &size, std::string icon)
{
	std::string fullPath = dirPath + "/" + name;
	oss << "<tr><td valign=\"top\"><img src=\"" << icon << "\" alt=\"[   ]\"></td>"
		<< "<td><a href=\"" << fullPath << "\">" << name << "</a></td>"
		<< "<td align=\"right\">" << modDate << "</td>"
		<< "<td align=\"right\">" << size << "</td>"
		<< "<td>&nbsp;</td></tr>\n";
}

void addFooter(std::ostringstream &oss)
{
	oss << "<tr><th colspan=\"5\"><hr></th></tr>\n"
		<< "</table>\n</body></html>";
}

bool isDirectory(const std::string &path)
{
	status statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

std::string findImage(std::string &name)
{
	if (isDirectory(name))
	{
		name += "/";
		return "/icons/folder.gif";
	}
	if (name.find(".html") != std::string::npos)
		return "/icons/html.gif";
	if (name.find(".png") != std::string::npos || name.find(".jpg") != std::string::npos || name.find(".jpeg") != std::string::npos)
		return "/icons/image.png";
	return "/icons/unknown.gif";
}

std::string resolveParentPath(const std::string &currentPath)
{
	if (currentPath.empty() || currentPath == "/")
		return "/";

	std::vector<std::string> parts;
	std::stringstream ss(currentPath);
	std::string part;
	while (std::getline(ss, part, '/'))
	{
		if (!part.empty())
			parts.push_back(part);
	}

	if (!parts.empty())
		parts.pop_back();

	std::string newPath;
	for (size_t i = 0; i < parts.size(); ++i)
		newPath += "/" + parts[i];
	return newPath.empty() ? "/" : newPath;
}

std::string listDirectory(const std::string &dirPath)
{
	DIR *dir = opendir(dirPath.c_str());
	if (!dir)
		return "<p>Erro ao abrir o diretório.</p>";

	std::ostringstream html;
	addHeader(html, dirPath);

	diretory *entry;
	addFileEntry(html, resolveParentPath(dirPath), "../", "-", "-", "/icons/folder.gif");

	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		std::string path = dirPath + "/" + name;
		status statbuf;
		stat(path.c_str(), &statbuf);
		std::string modDate = ctime(&statbuf.st_mtime);
		std::ostringstream size;
		size << statbuf.st_size;
		addFileEntry(html, dirPath, name, modDate, size.str(), findImage(name));
	}
	addFooter(html);
	closedir(dir);
	return html.str();
}
