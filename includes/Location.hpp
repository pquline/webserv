#pragma once

#include "webserv.hpp"

class	Location
{
	private:

		int									_autoindex;
		std::string							_root;
		std::map<unsigned int, std::string>	_error_pages;
		std::vector<std::string>			_indexes;
		std::vector<std::string>			_methods;
		std::map<std::string, std::string> _redirections;


	public:

		Location(int autoindex, std::string root, std::map<unsigned int, std::string> error_pages, \
			 std::vector<std::string> indexes, std::vector<std::string> methods, \
			 std::map<std::string, std::string> redirections);
		
		 ~Location(void);

		int	get_autoindex(void) const;

		bool isMethodAllowed(const std::string& method) const;
    	bool hasErrorPage(unsigned int code) const;
    	const std::string& getErrorPage(unsigned int code) const;
    	bool hasRedirection(const std::string& uri) const;
    	const std::string& getRedirection(const std::string& uri) const;
    	bool getAutoindex() const;

};
