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

class Server
{
	private:

		int m_port;
		int m_sockFd;
		struct sockaddr_in m_address;

	public:

		Server(int port);
		virtual ~Server ();

		void init();
		void run();
};

#endif /* SERVER_HPP */
