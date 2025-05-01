#include "webserv.hpp"

volatile sig_atomic_t g_sig = 0;

static void handle_sigint(int signal)
{
    if (signal == SIGINT)
        g_sig = 1;
}

static void	deleteServers(std::vector<Server *> &servers)
{
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
		delete(*it);
}

int	main(int argc, char **argv)
{
	std::vector<Server *>	servers;
	std::string				conf = DEFAULT_CONF;

	if (argc > 2)
	{
		std::cout << ERROR_USAGE << std::endl;
		return 0;
	}
	if (argc == 2)
		conf = argv[1];
	try
	{
		signal(SIGINT, handle_sigint);
		checkConfPathname(conf);
		parseConfigurationFile(conf, servers);
		std::map<int, Server*> fdToServer;
		std::set<int> listeningFds;

		int epollFd = epoll_create1(0);
		if (epollFd < 0)
			throw std::runtime_error("epoll_create1");

		for (size_t i = 0; i < servers.size(); ++i)
		{
			servers[i]->init();
			const std::vector<unsigned int> &fds = servers[i]->getServerFds();
			for (size_t j = 0; j < fds.size(); j++)
			{
				struct epoll_event ev;
				ev.events = EPOLLIN;
				ev.data.fd = fds[j];
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fds[j], &ev) < 0)
					throw std::runtime_error("epoll_ctl: server socket");
				fdToServer[fds[j]] = servers[i];
				listeningFds.insert(fds[j]);
			}
		}

		struct epoll_event ev, events[MAX_EVENT];
		int nfds, connFd;
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		while (true) {
			nfds = epoll_wait(epollFd, events, MAX_EVENT, 1000);

			if (g_sig)
			{
				std::cout << GREEN << "SIGINT detected, servers shutting down..." << RESET << std::endl;
				break;
			}
			if (nfds < 0)
				throw std::runtime_error("epoll_wait");
			for (int i = 0; i < nfds; i++)
			{
				int fd = events[i].data.fd;

				if (listeningFds.find(fd) != listeningFds.end())
				{
					Server* curServer = fdToServer[fd];
					socklen_t addrLen = sizeof(addr);
					connFd = accept(fd, (struct sockaddr *)&addr, &addrLen);
					if (connFd < 0)
					{
						std::cout << RED << "connFd < 0!" << RESET << std::endl;
						continue;
					}

					Server::setNonBlocking(connFd);
					ev.events = EPOLLIN | EPOLLET;
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
					char buffer[900000] = {0};
					ssize_t bytesRead = read(fd, buffer, 900000 - 1);
					if (bytesRead <= 0) {
						close(fd);
						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
						fdToServer.erase(fd);
						continue;
					}
					Server* curServer = fdToServer[fd];
					curServer->parseRequest(fd, bytesRead, buffer); 
					close(fd);
					epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
					fdToServer.erase(fd);
				}
			}
		}
		close(epollFd);

		deleteServers(servers);
		return (EXIT_SUCCESS);
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << ERROR_PREFIX << e.what() << RESET << std::endl;
		if (!servers.empty())
			deleteServers(servers);
		return (EXIT_FAILURE);
	}
}
