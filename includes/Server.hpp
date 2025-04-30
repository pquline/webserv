#pragma once

# include "webserv.hpp"
# include "HTTPRequest.hpp"
# include "Response.hpp"
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
		std::map<std::string, std::string>	_cookies;
		std::map<std::string, std::string> _redirections;

		void	handleError(const std::string& msg);
		void	setNonBlocking(int fd);
		void	parseRequest(int eventFd, ssize_t bytesRead,char *buffer);
		void	handlePostRequest(int eventFd, std::string& request);
		void	handleGetRequest(int eventFd, std::string& request);
		void	handleDeleteRequest(int eventFd, std::string& request);
		void	sendError(int fd, int code, const std::string& message);
		void	callCGI(int eventFd, std::string& request);
		int		get_autoindex(void) const;
		std::string	parseRequestTarget(const std::string& request);
		std::string	getHeader(const std::string& request, const std::string& key);
		const std::map<std::string, std::string> &getRedirections() const;
		const std::map<unsigned int, std::string> &getErrorPages() const;


	public:

		Server(int autoindex, ssize_t max_body_size, std::string root, \
				std::vector<std::string> hosts, std::vector<unsigned int> ports, \
				std::map<unsigned int, std::string> error_pages, \
				std::map<std::string, Location *> locations, 
				std::map<std::string, std::string> redirections);
		virtual ~Server ();

		void	init();
		void	run();
};
