#include "webserv.hpp"

void	parse_configuration_file(std::string file)
{
	if (file.size() < SIZE_EXTENSION)
		throw std::invalid_argument(ERROR_USAGE);
	if (static_cast<std::string>(&file[file.size() - SIZE_EXTENSION]).compare(CONF_EXTENSION) != 0)
		throw std::invalid_argument(ERROR_USAGE);
}

/*int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			parse_configuration_file(DEFAULT_CONF);
		else
			parse_configuration_file(argv[1]);
		return (EXIT_SUCCESS);
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << ERROR_PREFIX << e.what() << RESET << std::endl;
		return (EXIT_FAILURE);
	}*/
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: ./webserv port_to_listen" << std::endl;
		return 0;
	}
	int port = std::atoi(argv[1]);
	Server server((uint16_t)port);
	server.init();
	server.run();
	return 0;
}
