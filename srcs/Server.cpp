#include "Server.hpp"

Server::Server(const std::string& port) : m_port(port)
{
}

Server::~Server()
{
}

int init()
{
	int sockFd;
	struct sockaddr_in serverAddr;
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0)
	{
		std::cerr << "Failed to create server socket" << std::endl;
		exit(1);
	}

}
