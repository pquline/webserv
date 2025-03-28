#include "Server.hpp"

Server::Server(uint16_t port) : m_port(port), m_serverFd(-1)
{
	memset(&m_address, 0, sizeof(m_address));
}

Server::~Server()
{
	close(m_serverFd);
	std::cout << "Server shut down." << std::endl;
}

void Server::init()
{
	m_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_serverFd == -1)
		handleError("Socket creation failed");
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = htonl(INADDR_ANY);
	m_address.sin_port = htons(m_port);

	if (bind(m_serverFd, (struct sockaddr*)(&m_address), sizeof(m_address)) < 0)
		handleError("Binding failed");

	if (listen(m_serverFd, 10) < 0)
		handleError("Listening failed");
	std::cout << GREEN << "Server is listening on port " << m_port << "..." << RESET << std::endl;
}

void Server::run()
{
	int epollFd = epoll_create1(0);
	if (epollFd < 0)
		handleError("epoll_create1 failed");
	struct epoll_event ev, events[MAX_EVENT];
	ev.events = EPOLLIN;
	ev.data.fd = m_serverFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, m_serverFd, &ev) < 0)
		handleError("epoll_ctl: server socket");
	int nfds, connFd;
	while (true)
	{
		nfds = epoll_wait(epollFd, events, MAX_EVENT, -1);
		if (nfds < 0)
			handleError("epoll_wait");
		for (int i = 0; i < nfds; i++) {
			int eventFd = events[i].data.fd;
			if (eventFd == m_serverFd)
			{
				m_addressLen = sizeof(m_address);
				connFd = accept(m_serverFd, (struct sockaddr*)&m_address, &m_addressLen);
				if (connFd < 0)
					handleError("accept");
				setNonBlocking(connFd);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = connFd;
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, &ev) < 0)
					handleError("epoll_ctl: client socket");
			}
			else
			{
				char buffer[BUFFER_SIZE] = {0};
				//int bytesRead = read(eventFd, buffer, BUFFER_SIZE);

				if (read(eventFd, buffer, BUFFER_SIZE) < 0)
				{
					close(eventFd);
					epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
					continue;
				}
				std::cout << "Recieved request:\n" << buffer << std::endl;
			//	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 51\r\nContent-Type: text/html\r\n\r\n<h1>This is the beginning of my grand conquest!<h1>";
			/*	std::string response =
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 180\r\n"
					"\r\n"
					"<!DOCTYPE html>"
					"<html><body><h1>Hey, welcome to my homepage :)</h1>"
					"<form method='POST' action='/submit'>"
					"Name: <input type='text' name='name'><br>"
					"<input type='submit' value='Submit'>"
					"</form>"
					"</body></html>";*/
				/*std::ifstream file("../pages/default.html");
				std::string response((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());*/






				std::ifstream file("pages/test.html");
				if (!file)
				{
					close(eventFd);
					handleError("test.html");
				}

				std::stringstream buf;
				buf << file.rdbuf(); 
				file.close();

				std::string html = buf.str(); 

				std::ostringstream ss;
				ss << html.size();
				std::string strSize = ss.str();
				std::string response =
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: " + strSize + "\r\n"
					"\r\n" +
					html;

				send(eventFd, response.c_str(), response.size(), 0);
				send(connFd, response.c_str(), response.size(), 0);
				close(eventFd);
				epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
			}
		}
		/*	m_addressLen = sizeof(m_address);
			int connFd = accept(m_serverFd, (struct sockaddr*)&m_address, &m_addressLen);
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
			close(connFd);*/
	}
	close(epollFd);
}

void	Server::handleError(const std::string& msg)
{
	std::cerr << RED << msg << ": " << std::strerror(errno) << RESET << std::endl;
	if (m_serverFd != -1) close(m_serverFd);
	exit(EXIT_FAILURE);
}

void	Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
