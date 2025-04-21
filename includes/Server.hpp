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
# include "http_request.hpp"
# include "response.hpp"

# define MAX_EVENT 10
# define BUFFER_SIZE 1024

class Server
{
	private:

		uint16_t	m_port;
		int			m_serverFd;
		struct		sockaddr_in m_address;
		socklen_t	m_addressLen;

	public:

		Server(uint16_t port);
		virtual ~Server ();

		void	init();
		void	run();
		void	handleError(const std::string& msg);
		void	setNonBlocking(int fd);

		void	startParsing(int eventFd, ssize_t bytesRead,char *buffer);

		void	handlePostRequest(int eventFd, std::string& request);
		void	handleGetRequest(int eventFd, std::string& request);
		void	handleDeleteRequest(int eventFd, std::string& request);

		void	sendError(int fd, int code, const std::string& message);
};
#endif

std::string url_decode(const std::string& str);
std::map<std::string, std::string> parse_url_encoded(const std::string& body);
