#include "Location.hpp"

Location::~Location	(void)
{}

Location::Location(int autoindex, std::string root, std::map<unsigned int, std::string> error_pages, std::vector<std::string> indexes, std::vector<std::string> methods, std::map<std::string, std::string> redirections): _autoindex(autoindex), _root(root), _error_pages(error_pages), _indexes(indexes), _methods(methods), _redirections(redirections)
{}

int	Location::get_autoindex(void) const
{
	return (_autoindex);
}

bool Location::isMethodAllowed(const std::string& method) const 
{
    for (std::vector<std::string>::const_iterator it = _methods.begin(); it != _methods.end(); ++it) 
	{
        if (*it == method) 
		{
            return true;
        }
    }
    return false;
}

bool Location::hasErrorPage(unsigned int code) const {
    return _error_pages.find(code) != _error_pages.end();
}

const std::string& Location::getErrorPage(unsigned int code) const {
    return _error_pages.at(code);
}

bool Location::hasRedirection(const std::string& uri) const {
    return _redirections.find(uri) != _redirections.end();
}

const std::string& Location::getRedirection(const std::string& uri) const {
    return _redirections.at(uri);
}

bool Location::getAutoindex() const {
    return _autoindex;
}