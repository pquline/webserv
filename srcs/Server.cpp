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
	{
		std::cerr << RED << ERROR_PREFIX << "Socket creation failed." << RESET << std::endl;
		exit(EXIT_FAILURE);
	}
	// the sockadrr_in struct is used to define an IPv4 address for a socket
	/* struct sockaddr_in {
    	sa_family_t sin_family;  // Address family (always set to AF_INET for IPv4)
    	in_port_t sin_port;      // Port number (must be converted to network byte order using htons())
    	struct in_addr sin_addr; // IP address (struct with `s_addr`, converted with inet_addr() or INADDR_ANY to accept any connection)
	};*/
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = INADDR_ANY;
	m_address.sin_port = htons(m_port);

	if (bind(m_sockFd, (struct sockaddr*)(&m_address), sizeof(m_address)) == -1)
	{
		std::cerr << RED << ERROR_PREFIX << "Binding failed."<< RESET << std::endl;
		close(m_sockFd);
		exit(EXIT_FAILURE);
	}

	if (listen(m_sockFd, 10) == -1)
	{
		std::cerr << RED << ERROR_PREFIX << "Listening failed."<< RESET << std::endl;
		close(m_sockFd);
		exit(EXIT_FAILURE);
	}
	std::cout << GREEN << "Server is listening on port " << m_port << "..." << RESET << std::endl;
}

void Server::run()
{
    while (true)
	{
		m_addressLen = sizeof(m_address);
        int clientFd = accept(m_sockFd, (struct sockaddr*)&m_address, &m_addressLen);
        if (clientFd == -1) {
            std::cerr << "Error: Accepting connection failed" << std::endl;
            continue;
        }

        std::cout << "New client connected!" << std::endl;
		const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 43\r\nContent-Type: text/plain\r\n\r\nThis is the beginning of my grand conquest!";
		write(clientFd, response, strlen(response));
        close(clientFd);
    }
}
