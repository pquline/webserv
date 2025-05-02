#include "webserv.hpp"

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
		std::cerr << ERROR_USAGE << std::endl;
		return 0;
	}
	if (argc == 2)
		conf = argv[1];
	try
	{
		checkConfPathname(conf);
		parseConfigurationFile(conf, servers);
		deleteServers(servers);
		return (EXIT_SUCCESS);
	}
	catch (const std::exception &e)
	{
		logWithTimestamp(e.what(), RED);
		if (!servers.empty())
			deleteServers(servers);
		return (EXIT_FAILURE);
	}
}
