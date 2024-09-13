#include "Request.hpp"

bool Request::parseRequest(const std::string &raw_request)
{
    std::istringstream stream(raw_request);
    stream >> method >> uri >> http_version;
    this->allow_directory_listing = true;
    if (method.empty() || uri.empty() || http_version.empty())
    {
        return false;
    }
    return true;
}
Request::Request() : method(""), uri(""), http_version("") {}

