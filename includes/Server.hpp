#ifndef SERVER_HPP
#define SERVER_HPP

# include <iostream>
# include <vector>
# include <cstdlib>
# include <string>
# include <cstring>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <unistd.h>
# include <sstream>
# include <sstream>
# include "webserv.hpp"
# include "Location.hpp"

# define MAX_EVENT 10
# define BUFFER_SIZE 1024

class Server
{
	private:

		uint16_t	m_port;
		int			m_serverFd;
		struct		sockaddr_in m_address;
		socklen_t	m_addressLen;
		int									_autoindex;
		ssize_t								_max_body_size;
		std::string							_root;
		std::vector<std::string>			_hosts;
		std::vector<unsigned int>			_ports;
		std::map<unsigned int, std::string>	_error_pages;
		std::map<std::string, Location *>	_locations;

	public:

		Server(int autoindex, ssize_t max_body_size, std::string root, \
				std::vector<std::string> hosts, std::vector<unsigned int> ports, \
				std::map<unsigned int, std::string> error_pages, \
				std::map<std::string, Location *> locations);
		virtual ~Server ();

		void	init();
		void	run();
		void	handleError(const std::string& msg);
		void	setNonBlocking(int fd);
		int		get_autoindex(void) const;
};

#endif /* SERVER_HPP */
