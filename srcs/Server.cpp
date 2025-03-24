#include "Server.hpp"

Server::Server(uint16_t port) : m_port(port), m_sockFd(-1)
{
	memset(&m_address, 0, sizeof(m_address));
}

Server::~Server()
{
	close(m_sockFd);
	std::cout << "Server shut down." << std::endl;
}

void Server::init()
{
	m_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	// socket(int domain, int type, int protocol), creates a socket and returns the fd to manage the socket
	// AF_INET -> IPv4, AF_INET6 -> IPv6
	// SOCK_STREAM -> TCP, SOCK_DGRAM -> UDP
	// 0 -> the default protocol for the given type
	if (m_sockFd == -1)
		handleError("Socket creation failed");
	// the sockadrr_in struct is used to define an IPv4 address for a socket
	/* struct sockaddr_in {
    	sa_family_t sin_family;  // Address family (always set to AF_INET for IPv4)
    	in_port_t sin_port;      // Port number (must be converted to network byte order using htons())
    	struct in_addr sin_addr; // IP address (struct with `s_addr`, converted with inet_addr() or INADDR_ANY to accept any connection)
	};*/
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = htonl(INADDR_ANY);
	m_address.sin_port = htons(m_port);

	if (bind(m_sockFd, (struct sockaddr*)(&m_address), sizeof(m_address)) < 0)
		handleError("Binding failed");

	if (listen(m_sockFd, 10) < 0)
		handleError("Listening failed");
	std::cout << GREEN << "Server is listening on port " << m_port << "..." << RESET << std::endl;
}

void Server::run()
{
	int maxFd = m_sockFd;
	fd_set currentSockets, readySockets;
	FD_ZERO(&currentSockets);
	FD_SET(m_sockFd, &currentSockets);
    while (true)
	{
		readySockets = currentSockets;
		if (select(maxFd + 1, &readySockets, nullptr, nullptr, nullptr) < 0)
		{
			if (errno == EINTR) continue;
			handleError("Select failed");
		}
		if (FD_ISSET(m_sockFd, &readySockets))
		{
			int connFd = accept(m_sockFd, nullptr, nullptr);
			if (connFd < 0)
			{
				std::cerr << RED << "Failed to accept connection: " << strerror(errno) << RESET << std::endl;
				continue;
			}
			std::cout << "New client connected!" << std::endl;
			FD_SET(connFd, &currentSockets);
			maxFd = std::max(maxFd, connFd);
		}
		for (int i = 0; i <= maxFd; i++) {
			if (i != m_sockFd && FD_ISSET(i, &readySockets))
			{

			}
		}
		/*m_addressLen = sizeof(m_address);
        int connFd = accept(m_sockFd, (struct sockaddr*)&m_address, &m_addressLen);
        if (connFd < 0)
		{
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			else
				handleError("Accepting connection failed");
        }

        std::cout << "New client connected!" << std::endl;
		const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 51\r\nContent-Type: text/html\r\n\r\n<h1>This is the beginning of my grand conquest!<h1>";
		write(connFd, response, strlen(response));
        close(connFd);*/
    }
}

void Server::handleError(const std::string& msg)
{
	std::cerr << RED << msg << ": " << std::strerror(errno) << RESET << std::endl;
	if (m_sockFd != -1) close(m_sockFd);
	exit(EXIT_FAILURE);
}
