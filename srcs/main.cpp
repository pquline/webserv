#include "webserv.hpp"

volatile sig_atomic_t g_sig = 0;
volatile int g_epollfd = 0;

static void handle_sigint(int signal)
{
    (void)signal;
    g_sig = 1;
}

static void deleteServers(std::vector<Server *> &servers)
{
    for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::vector<int> sockets = (*it)->getServerFds();
		for (size_t i = 0; i < sockets.size(); i++)
			close(sockets[i]);
        delete (*it);
	}
}

static bool isCompleteRequest(const std::string &req, size_t &bodySize)
{
    size_t headerEnd = req.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
    {
        return false;
    }

    std::string headers = req.substr(0, headerEnd);

    if (headers.find("Content-Length:") != std::string::npos)
    {
        size_t pos = headers.find("Content-Length:");
        size_t end = headers.find("\r\n", pos);
        std::string contentLengthStr = headers.substr(pos + 15, end - pos - 15);
        int contentLength = std::atoi(contentLengthStr.c_str());

        bodySize = static_cast<size_t>(contentLength);
        size_t totalExpectedSize = headerEnd + 4 + bodySize;

        return req.size() >= totalExpectedSize;
    }

    return true;
}

int main(int argc, char **argv)
{
    std::vector<Server *> servers;
    std::string conf = DEFAULT_CONF;

    if (argc > 2)
    {
        std::cerr << ERROR_USAGE << std::endl;
        return 0;
    }
    if (argc == 2)
        conf = argv[1];
    try
    {
        signal(SIGINT, handle_sigint);
        checkConfPathname(conf);
        parseConfigurationFile(conf, servers);
        std::map<int, Server *> fdToServer;
        std::set<int> listeningFds;
        std::map<int, std::string> requestBuffer;

        int epollFd = epoll_create1(0);
		g_epollfd = epollFd;
        if (epollFd < 0)
            throw std::runtime_error("epoll_create1");

        for (size_t i = 0; i < servers.size(); ++i)
        {
            servers[i]->init();
            const std::vector<int> &fds = servers[i]->getServerFds();
            for (size_t j = 0; j < fds.size(); j++)
            {
                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = fds[j];
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fds[j], &ev) < 0)
				{
					close(epollFd);
                    throw std::runtime_error("epoll_ctl: server socket");
				}
                fdToServer[fds[j]] = servers[i];
                listeningFds.insert(fds[j]);
            }
        }

        struct epoll_event ev, events[MAX_EVENT];
        int nfds, connFd;
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        while (true)
        {
            nfds = epoll_wait(epollFd, events, MAX_EVENT, 1000);

            if (g_sig)
            {
                std::cerr << std::endl;
                logWithTimestamp("SIGINT detected, servers shutting down...", RED);
                break;
            }
            if (nfds < 0)
			{
				close(epollFd);
				throw std::runtime_error("epoll_wait");
			}
			for (int i = 0; i < nfds; i++)
			{
				int fd = events[i].data.fd;

				if (listeningFds.find(fd) != listeningFds.end())
				{
					Server *curServer = fdToServer[fd];
					socklen_t addrLen = sizeof(addr);
					connFd = accept(fd, (struct sockaddr *)&addr, &addrLen);
					if (connFd < 0)
					{
						std::cout << RED << "connFd < 0!" << RESET << std::endl;
						continue;
					}

					Server::setNonBlocking(connFd);
					ev.events = EPOLLIN;
					ev.data.fd = connFd;
					if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, &ev) < 0)
					{
						std::cout << RED << "epoll_ctl() < 0!" << RESET << std::endl;
						close(connFd);
						continue;
					}
					fdToServer[connFd] = curServer;
				}
				else
				{
					char buffer[BUFFER_SIZE] = {0};
					ssize_t bytesRead = read(fd, buffer, BUFFER_SIZE);
					if (bytesRead < 0)
					{
						logWithTimestamp("Error reading from socket: " + std::string(strerror(errno)), RED);
						requestBuffer.erase(fd);
						close(fd);
						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
						fdToServer.erase(fd);
						continue;
					}
					if (bytesRead == 0)
					{
						logWithTimestamp("Client closed connection", YELLOW);
						requestBuffer.erase(fd);
						close(fd);
						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
						fdToServer.erase(fd);
						continue;
					}
					requestBuffer[fd] += std::string(buffer, static_cast<size_t>(bytesRead));
					Server *curServer = fdToServer[fd];
					size_t bodySize = 0;
					if (isCompleteRequest(requestBuffer[fd], bodySize))
					{
						std::string response = curServer->parseRequest(requestBuffer[fd]);
						ssize_t bytesSent = 0;
						ssize_t totalBytes = static_cast<long>(response.size());
						const char *responsePtr = response.c_str();

						while (bytesSent < totalBytes)
						{
							ssize_t sent = send(fd, responsePtr + bytesSent, static_cast<unsigned long>(totalBytes - bytesSent), 0);
							if (sent < 0)
							{
								logWithTimestamp("Error sending response: " + std::string(strerror(errno)), RED);
								break;
							}
							if (sent == 0)
							{
								logWithTimestamp("Connection closed while sending response", YELLOW);
								break;
							}
							bytesSent += sent;
						}
						requestBuffer.erase(fd);
						close(fd);
						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
						fdToServer.erase(fd);
					}
				}
			}
		}
		close(epollFd);
		deleteServers(servers);
		return (EXIT_SUCCESS);
	}
	catch (const std::exception &e)
	{
		logWithTimestamp(e.what(), RED);
		close(g_epollfd);
		if (!servers.empty())
			deleteServers(servers);
		return (EXIT_FAILURE);
	}
}
