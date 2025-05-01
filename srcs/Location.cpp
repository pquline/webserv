#include "Location.hpp"

Location::~Location	(void)
{}

Location::Location(int autoindex, std::string root, std::map<unsigned int, std::string> error_pages, std::vector<std::string> indexes, std::vector<std::string> methods): _autoindex(autoindex), _root(root), _error_pages(error_pages), _indexes(indexes), _methods(methods)
{}

int	Location::get_autoindex(void) const
{
	return (_autoindex);
}
