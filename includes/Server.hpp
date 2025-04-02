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
		void 	handlePostRequest(int eventFd, const std::string& request);
		void 	handleGetRequest(int eventFd, const std::string& request);
		void 	handleDeleteRequest(int eventFd, const std::string& request);

		void	handleErrorResponse(int eventFd, const std::string& error);
};
#endif 
