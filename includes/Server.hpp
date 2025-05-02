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

		int									_autoindex;
		ssize_t								_max_body_size;
		std::string							_root;
		std::vector<std::string>			_hosts;
		std::vector<unsigned int>			_ports;
		std::vector<int>					_serverFds;
		std::map<unsigned int, std::string>	_error_pages;
		std::map<std::string, Location *>	_locations;
		std::map<std::string, std::string>	_cookies;
		std::map<std::string, std::string>  _redirections;

		void	handlePostRequest(int eventFd, const std::string& request);
		void	handleGetRequest(int eventFd, const std::string& request);
		void	handleDeleteRequest(int eventFd, const std::string& request);
		void	sendError(int fd, int code, const std::string& message);
		void	callCGI(int eventFd, const std::string& request);
		std::string	parseRequestTarget(const std::string& request);
		std::string	getHeader(const std::string& request, const std::string& key);
		Location* getExactLocation(const std::string& uri) const;
		void sendRedirection(int eventFd, const std::string& destination, int code);


	public:

		Server(int autoindex, ssize_t max_body_size, std::string root, \
				std::vector<std::string> hosts, std::vector<unsigned int> ports, \
				std::map<unsigned int, std::string> error_pages, \
				std::map<std::string, Location *> locations,
				std::map<std::string, std::string> redirections);
		virtual ~Server ();

		void	init();
		void	run();
		int		get_autoindex(void) const;
		const std::map<std::string, std::string> &getRedirections() const;
		const std::map<unsigned int, std::string> &getErrorPages() const;
		const std::vector<int> getServerFds() const;
		static void	setNonBlocking(int fd);
		void	parseRequest(int eventFd, const std::string &request);
};
