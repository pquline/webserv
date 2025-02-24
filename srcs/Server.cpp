#include "Server.hpp"

Server::Server(int port) : m_port(port)
{
	memset(&m_address, 0, sizeof(address));
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
	{
		std::cerr << "Error: Socket creation failed." << std::endl;
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in address;
	// the sockadrr_in struct is used to define an IPv4 address for a socket
	/* struct sockaddr_in {
    	sa_family_t sin_family;  // Address family (always set to AF_INET for IPv4)
    	in_port_t sin_port;      // Port number (must be converted to network byte order using htons())
    	struct in_addr sin_addr; // IP address (struct with `s_addr`, converted with inet_addr() or INADDR_ANY to accept any connection)
	};*/
	address.sin_family = AT_INET;
	address.sin_port = htons(m_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_sockFd), dynamic_cast<struct sockaddr*>(&m_address), sizeof(m_address) == -1)
	{
		std::cerr << "Error: Binding failed." << std::endl;
		close(m_sockFd);
		exit(EXIT_FAILURE)
	}

	if (listen(m_sockFd, 10) == -1)
	{
		std::cerr << "Error: Listending failed." << std::endl;
		close(m_sockFd);
		exit(EXIT_FAILURE)
	}
	std::cout << GREEN << "Server is listening on port " << m_port << RESET << std::endl;
}

void server::run()
{
    while (true)
	{
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientFd = accept(m_sockFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd == -1) {
            std::cerr << "Error: Accepting connection failed" << std::endl;
            continue;
        }

        std::cout << "New client connected!" << std::endl;

        close(clientFd);
    }
}
