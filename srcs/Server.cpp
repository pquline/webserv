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
                ssize_t bytesRead = read(eventFd, buffer, BUFFER_SIZE - 1);
                
                if (bytesRead <= 0)
                {
                    close(eventFd);
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
                    continue;
                }

                std::string request(buffer, static_cast<size_t>(bytesRead));
    			std::string requestedPath = "/interactivepage.html"; 

                size_t start = request.find(' ') + 1;
                size_t end = request.find(' ', start);
                if (start != std::string::npos && end != std::string::npos)
                {
                    requestedPath = request.substr(start, end - start);
                    if (requestedPath == "/") 
                        requestedPath = "/interactivepage.html";
                }

                std::string filePath = "pages" + requestedPath;

                std::ifstream file(filePath.c_str());
                if (!file)
                {
                    std::string notFound = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
                    send(eventFd, notFound.c_str(), notFound.size(), 0);
                }
                else
                {
                    std::stringstream buf;
                    buf << file.rdbuf();
                    file.close();

                    std::string content = buf.str();
                    std::ostringstream ss;
                    ss << content.size();

                    std::string contentType = "text/html";
                    if (requestedPath.find(".css") != std::string::npos)
                        contentType = "text/css";
                    else if (requestedPath.find(".js") != std::string::npos)
                        contentType = "application/javascript";
                    else if (requestedPath.find(".png") != std::string::npos)
                        contentType = "image/png";
                    else if (requestedPath.find(".jpg") != std::string::npos)
                        contentType = "image/jpeg";

                    std::string response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Length: " + ss.str() + "\r\n"
                        "Content-Type: " + contentType + "\r\n\r\n" + content;

                    send(eventFd, response.c_str(), response.size(), 0);
                }

                close(eventFd);
                epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
            }
        }
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
	if (flags == -1) handleError("fcntl F_GETFL");

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) handleError("fcntl F_SETFL");
}
