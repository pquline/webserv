#include "webserv.hpp"

void handleError(const std::string& msg)
{
	std::cerr << RED << msg << ": " << std::strerror(errno) << RESET << std::endl;
	exit(EXIT_FAILURE);
}
