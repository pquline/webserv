#include "webserv.hpp"

static void	delete_servers(std::vector<Server *> &servers)
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
		check_conf_pathname(conf);
		parse_conf_file(conf, servers);
		delete_servers(servers);
		return (EXIT_SUCCESS);
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << ERROR_PREFIX RESET << e.what() << std::endl;
		if (!servers.empty())
			delete_servers(servers);
		return (EXIT_FAILURE);
	}
}
/*
int main(int argc, char** argv)
{
	int port = std::atoi(argv[1]);
	Server server((uint16_t)port);
	server.init();
	server.run();
	return 0;
}*/
