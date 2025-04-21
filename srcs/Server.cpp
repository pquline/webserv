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

                char buffer[900000] = {0};
                ssize_t bytesRead = read(eventFd, buffer, 900000 - 1);

                if (bytesRead <= 0)
                {
                    close(eventFd);
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
                    continue;
                }
                Server::startParsing(eventFd, bytesRead, buffer);

                close(eventFd);
                epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
            }
        }
    }
    close(epollFd);
}

static std::string generateErrorPage(int code, const std::string& message) {
    std::ostringstream page;
    page << "<!DOCTYPE html>\n"
         << "<html lang=\"en\">\n"
         << "<head>\n"
         << "    <meta charset=\"UTF-8\">\n"
         << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
         << "    <title>Error " << code << "</title>\n"
         << "    <style>\n"
         << "        body {\n"
         << "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
         << "            display: flex;\n"
         << "            justify-content: center;\n"
         << "            align-items: center;\n"
         << "            height: 100vh;\n"
         << "            margin: 0;\n"
         << "            background-color: #ffffff;\n"
         << "            color: #ecf0f1;\n"
         << "            text-align: center;\n"
         << "        }\n"
         << "        .error-container {\n"
         << "            background-color: #34495e;\n"
         << "            padding: 40px;\n"
         << "            border-radius: 10px;\n"
         << "            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);\n"
         << "            max-width: 400px;\n"
         << "            width: 100%;\n"
         << "        }\n"
         << "        h1 {\n"
         << "            font-size: 6rem;\n"
         << "            color: #e74c3c;\n"
         << "            margin: 0;\n"
         << "        }\n"
         << "        p {\n"
         << "            font-size: 1.2rem;\n"
         << "            color: #bdc3c7;\n"
         << "            margin-top: 15px;\n"
         << "        }\n"
         << "        .message {\n"
         << "            font-weight: bold;\n"
         << "            color: #ecf0f1;\n"
         << "        }\n"
         << "        a {\n"
         << "            display: inline-block;\n"
         << "            margin-top: 30px;\n"
         << "            padding: 12px 24px;\n"
         << "            background-color: #3498db;\n"
         << "            color: white;\n"
         << "            text-decoration: none;\n"
         << "            border-radius: 4px;\n"
         << "            font-size: 1rem;\n"
         << "            transition: background-color 0.3s;\n"
         << "        }\n"
         << "        a:hover {\n"
         << "            background-color: #2980b9;\n"
         << "        }\n"
         << "        a:active {\n"
         << "            background-color: #1c638d;\n"
         << "        }\n"
         << "    </style>\n"
         << "</head>\n"
         << "<body>\n"
         << "    <div class=\"error-container\">\n"
         << "        <h1>" << code << "</h1>\n"
         << "        <p class=\"message\">" << message << "</p>\n"
         << "        <a href=\"/\">Back to Home</a>\n"
         << "    </div>\n"
         << "</body>\n"
         << "</html>\n";

    return page.str();
}

void	Server::sendError(int fd, int code, const std::string& message)
{
    std::string body = generateErrorPage(code, message);
    std::ostringstream response;

	std::cerr << BLUE "[ERROR]" RESET ": " <<  body << std::endl;
    response << "HTTP/1.1 " << code << " " << message << "\r\n"
             << "Content-Type: text/html\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n"
             << body;
    send(fd, response.str().c_str(), response.str().size(), 0);
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
	if (flags == -1) handleError("fcntl F_GETFL");

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) handleError("fcntl F_SETFL");
}

