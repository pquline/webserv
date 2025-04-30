#pragma once

#include "webserv.hpp"

class	Location
{
	private:

		int									_autoindex;
		std::string							_root;
		std::map<unsigned int, std::string>	_error_pages;
		std::vector<std::string>			_indexes;

	public:

		Location(int autoindex, std::string root, std::map<unsigned int, std::string> error_pages, std::vector<std::string> indexes);
		~Location(void);

		int	get_autoindex(void) const;

};
