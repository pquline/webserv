#ifndef SERVER_HPP
#define SERVER_HPP

# include <iostream>
# include <vector>
# include <cstdlib>
# include <string>
# include <cstdio>
# include <cstring>
# include <fcntl.h>
# include <fstream>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <unistd.h>
# include <sstream>
# include "webserv.hpp"
# include "http_request.hpp"
# include "response.hpp"
# include "Location.hpp"

# define MAX_EVENT 10
# define BUFFER_SIZE 1024

class Location;

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

		void	handleError(const std::string& msg);
		void	setNonBlocking(int fd);
		void	startParsing(int eventFd, ssize_t bytesRead,char *buffer);
		void	handlePostRequest(int eventFd, std::string& request);
		void	handleGetRequest(int eventFd, std::string& request);
		void	handleDeleteRequest(int eventFd, std::string& request);
		void	sendError(int fd, int code, const std::string& message);
		int		get_autoindex(void) const;
		std::string	parseRequestTarget(const std::string& request); 
		std::string	getHeader(const std::string& request, const std::string& key); 


	public:

		Server(int autoindex, ssize_t max_body_size, std::string root, \
				std::vector<std::string> hosts, std::vector<unsigned int> ports, \
				std::map<unsigned int, std::string> error_pages, \
				std::map<std::string, Location *> locations);
		virtual ~Server ();

		void	init();
		void	run();
};
#endif

std::string url_decode(const std::string& str);
std::map<std::string, std::string> parse_url_encoded(const std::string& body);
