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
	if (m_sockFd == -1)
		handleError("Socket creation failed");
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
	while (true)
	{
		m_addressLen = sizeof(m_address);
		int connFd = accept(m_sockFd, (struct sockaddr*)&m_address, &m_addressLen);
		if (connFd < 0)
		{
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			else
				handleError("Accepting connection failed");
		}

		std::cout << "New client connected!" << std::endl;
		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 51\r\nContent-Type: text/html\r\n\r\n<h1>This is the beginning of my grand conquest!<h1>";
		send(connFd, response.c_str(), response.size(), 0);
		close(connFd);
	}
}

void Server::handleError(const std::string& msg)
{
	std::cerr << RED << msg << ": " << std::strerror(errno) << RESET << std::endl;
	if (m_sockFd != -1) close(m_sockFd);
	exit(EXIT_FAILURE);
}
