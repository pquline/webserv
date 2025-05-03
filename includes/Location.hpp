#pragma once

#include "webserv.hpp"

class	Location
{
	private:

		int									_autoindex;
		std::string							_root;
		std::vector<std::string>			_methods;
		std::map<std::string, std::string> _redirections;
		std::vector<std::string>			_indexes;

	public:

		Location(int autoindex, std::string root, \
			 std::vector<std::string> methods, \
			 std::map<std::string, std::string> redirections, std::vector<std::string> indexes);

		 ~Location(void);

		int	get_autoindex(void) const;

		bool isMethodAllowed(const std::string& method) const;
    	bool hasErrorPage(unsigned int code) const;
    	const std::string& getErrorPage(unsigned int code) const;
    	bool hasRedirection(const std::string& uri) const;
    	const std::string& getRedirection(const std::string& uri) const;
    	bool getAutoindex() const;
		const std::vector<std::string>& getIndexes() const;

};
