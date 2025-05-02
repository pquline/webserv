#include "Server.hpp"

void checkConfPathname(const std::string &pathname)
{
	size_t n = pathname.size();
	if (n < 5 || pathname.substr(n - 5) != CONF_EXTENSION)
		throw std::invalid_argument(ERROR_USAGE);
}

static std::string getConfContent(const std::string &file)
{
	std::ifstream conf(file.c_str());
	std::stringstream buffer;

	if (conf.is_open() == false)
		throw std::invalid_argument(ERROR_OPEN + file);

	buffer << conf.rdbuf();
	conf.close();
	return (buffer.str());
}

static std::string getLocationBlock(const std::string &block, size_t &index)
{
	size_t begin;
	size_t curly_brackets;
	std::string location_block;

	index += SIZE_LOCATION;
	while (isspace(block[index]))
		index++;
	begin = index;
	while (index < block.size() && !isspace(block[index]) && block[index] != '{')
		index++;
	while (isspace(block[index]))
		index++;
	if (block[index] != '{')
		throw std::invalid_argument(PARSING_UNEXPECTED);
	curly_brackets = 0;
	while (index < block.size() && (curly_brackets > 0 || block[index] == '{'))
	{
		if (block[index] == '{')
			curly_brackets++;
		else if (block[index] == '}')
			curly_brackets--;
		index++;
	}
	if (curly_brackets != 0)
		throw std::invalid_argument(PARSING_UNMATCHED_BRACKET);
	location_block = block.substr(begin, index - begin);
	return (location_block);
}

static void parseMethods(const std::string &directive, std::vector<std::string> &methods)
{
    size_t index = std::string("methods").size();
    size_t begin;

    while (isspace(directive[index]))
        index++;
    while (index < directive.size())
    {
        begin = index;
        while (index < directive.size() && !isspace(directive[index]))
            index++;
        std::string method = directive.substr(begin, index - begin);
        for (size_t i = 0; i < method.size(); ++i) {
            method[i] = static_cast<char>(toupper(static_cast<unsigned char>(method[i])));
        }
        methods.push_back(method);
        while (isspace(directive[index]))
            index++;
    }

    std::cerr << CONF_PREFIX << "methods: [";
    for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
    {
        std::vector<std::string>::const_iterator temp = ++it;
        --it;
        std::cerr << *it;
        if (temp != methods.end())
            std::cerr << ", ";
    }
    std::cerr << "]" << std::endl;
}

static std::string getServerBlock(const std::string &block, size_t &index)
{
	size_t begin;
	size_t curly_brackets;
	std::string server_block;

	index += SIZE_SERVER;
	while (isspace(block[index]))
		index++;
	if (block[index] != '{')
		throw std::invalid_argument(PARSING_UNEXPECTED);
	curly_brackets = 0;
	begin = index + 1;
	while (index < block.size() && (curly_brackets > 0 || index == begin - 1))
	{
		if (block[index] == '{')
			curly_brackets++;
		else if (block[index] == '}')
			curly_brackets--;
		index++;
	}
	if (curly_brackets != 0)
		throw std::invalid_argument(PARSING_UNMATCHED_BRACKET);
	server_block = block.substr(begin, index - begin - 1);
	return (server_block);
}

static std::string getDirective(const std::string &data, size_t &index)
{
	size_t begin = index;
	std::string directive;

	while (index < data.size() && data[index] != ';')
		index++;
	if (data[index] != ';')
		throw std::invalid_argument(PARSING_UNMATCHED_BRACKET);
	directive = data.substr(begin, index - begin);
	return (directive);
}

static void parseAutoindex(const std::string &directive, int &autoindex)
{
	size_t index = std::string("autoindex").size();

	while (index < directive.size() && isspace(directive[index]))
		index++;
	if (strncmp(&directive[index], "on", 2) == 0)
	{
		autoindex = true;
		index += 2;
	}
	else if (strncmp(&directive[index], "off", 3) == 0)
	{
		autoindex = false;
		index += 3;
	}
	else
		throw std::invalid_argument(PARSING_AUTOINDEX);
	while (index < directive.size() && isspace(directive[index]))
		index++;
	if (index < directive.size() && directive[index] != ';')
		throw std::invalid_argument(PARSING_AUTOINDEX);
	std::cerr << CONF_PREFIX << "autoindex: [";
	if (autoindex == 1)
		std::cerr << "on";
	else
		std::cerr << "off";
	std::cerr << "]" << std::endl;
}

static void parseMaxBodySize(const std::string &directive, ssize_t &max_body_size)
{
	size_t index = std::string("client_max_body_size").size();
	size_t begin;

	while (index < directive.size() && isspace(directive[index]))
		index++;
	begin = index;
	while (index < directive.size() && isdigit(directive[index]))
		index++;
	if (begin == index || index - begin > 9)
		throw std::invalid_argument(PARSING_BODY_SIZE);
	max_body_size = strtol((directive.substr(begin, index - begin)).c_str(), nullptr, 10);
	std::cerr << CONF_PREFIX << "client_max_body_size: [" << max_body_size << "]" << std::endl;
}

static void parseRoot(const std::string &directive, std::string &root)
{
	size_t begin;
	size_t index = std::string("root").size();

	while (isspace(directive[index]))
		index++;
	begin = index;
	while (index < directive.size() && !isspace(directive[index]) &&
		   directive[index] != ';')
		index++;
	root = directive.substr(begin, index - begin);
	std::cerr << CONF_PREFIX << "root: [" << root << "]" << std::endl;
}

static void parseHost(const std::string &directive, std::vector<std::string> &hosts)
{
	size_t index = std::string("server_name").size();

	while (isspace(directive[index]))
		index++;
	while (index < directive.size())
	{
		size_t end = index;
		while (end < directive.size() && !isspace(directive[end]))
			end++;
		hosts.push_back(directive.substr(index, end - index));
		while (isalpha(directive[index]) || ispunct(directive[index]))
			index++;
		while (isspace(directive[index]))
			index++;
	}
	std::cerr << CONF_PREFIX << "server_name: [";
	for (std::vector<std::string>::const_iterator it = hosts.begin(); it != hosts.end(); it++)
	{
		std::vector<std::string>::const_iterator temp = ++it;
		--it;
		std::cerr << *it;
		if (temp != hosts.end())
			std::cerr << ", ";
	}
	std::cerr << "]" << std::endl;
}

static void parsePort(const std::string &directive, std::vector<unsigned int> &ports)
{
	size_t index = std::string("listen").size();
	unsigned int port;

	while (isspace(directive[index]))
		index++;
	while (index < directive.size())
	{
		size_t end = index;
		while (end < directive.size() && !isspace(directive[end]))
			end++;
		port = static_cast<unsigned int>(strtol((directive.substr(index, end - index)).c_str(), nullptr, 10));
		if (port >= 2 << 16)
			throw std::invalid_argument(PARSING_PORTS);
		ports.push_back(port);
		while (isdigit(directive[index]))
			index++;
		while (isspace(directive[index]))
			index++;
		if (isalpha(directive[index]))
			throw std::invalid_argument(PARSING_PORTS);
	}
	std::cerr << CONF_PREFIX << "ports: [";
	for (std::vector<unsigned int>::const_iterator it = ports.begin(); it != ports.end(); it++)
	{
		std::vector<unsigned int>::const_iterator temp = ++it;
		--it;
		std::cerr << *it;
		if (temp != ports.end())
			std::cerr << ", ";
	}
	std::cerr << "]" << std::endl;
}

static void parseErrorPage(const std::string &directive,
						   std::map<unsigned int, std::string> &error_pages)
{
	size_t index = std::string("error_page").size();
	size_t begin;
	unsigned int error_code;
	std::string path;

	while (isspace(directive[index]))
		index++;
	begin = index;
	while (isdigit(directive[index]))
		index++;
	if (begin == index)
		throw std::invalid_argument(PARSING_ERROR_PAGE);
	error_code = static_cast<unsigned int>(strtol(directive.substr(begin, index - begin).c_str(), nullptr, 10));
	if (error_code > 527)
		throw std::invalid_argument(PARSING_ERROR_PAGE);
	while (isspace(directive[index]))
		index++;
	begin = index;
	while (isalpha(directive[index]) || ispunct(directive[index]) || isdigit(directive[index]) || directive[index] == '/')
		index++;
	if (begin == index)
		throw std::invalid_argument(PARSING_ERROR_PAGE);
	path = directive.substr(begin, index - begin);
	error_pages[error_code] = path;
	std::cerr << CONF_PREFIX << "error_pages: [";
	for (std::map<unsigned int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); it++)
	{
		std::map<unsigned int, std::string>::const_iterator temp = ++it;
		--it;
		std::cerr << it->first << ": " << it->second;
		if (temp != error_pages.end())
			std::cerr << ", ";
	}
	std::cerr << "]" << std::endl;
}

static void parseIndexes(const std::string &directive, std::vector<std::string> &indexes)
{
	size_t index = std::string("index").size();
	size_t begin;

	while (isspace(directive[index]))
		index++;
	while (index < directive.size())
	{
		begin = index;
		while (index < directive.size() && !isspace(directive[index]))
			index++;
		indexes.push_back(directive.substr(begin, index - begin));
		while (isspace(directive[index]))
			index++;
	}
	std::cerr << CONF_PREFIX << "indexes: [";
	for (std::vector<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); it++)
	{
		std::vector<std::string>::const_iterator temp = ++it;
		--it;
		std::cerr << *it;
		if (temp != indexes.end())
			std::cerr << ", ";
	}
	std::cerr << "]" << std::endl;
}

static std::string getDirectiveKey(const std::string &directive, size_t index)
{
	const size_t begin = index;
	std::string key;

	while (index < directive.size() && !isspace(directive[index]))
		index++;
	if (!isspace(directive[index]))
		throw std::invalid_argument(PARSING_UNEXPECTED);
	key = directive.substr(begin, index - begin);
	return (key);
}

static void parseRedirection(const std::string &directive, std::map<std::string, std::string> &redirections)
{
    size_t index = std::string("redirection").size();
    size_t begin;
    std::string from;
    std::string to;

    while (isspace(directive[index]))
        index++;

    begin = index;
    while (index < directive.size() && !isspace(directive[index]))
        index++;
    from = directive.substr(begin, index - begin);

    while (isspace(directive[index]))
        index++;

    begin = index;
    while (index < directive.size() && !isspace(directive[index]) && directive[index] != ';')
        index++;
    to = directive.substr(begin, index - begin);

    redirections[from] = to;

    std::cerr << CONF_PREFIX << "redirection: [" << from << " -> " << to << "]" << std::endl;
}

static void parseLocation(const std::string &directive,
						  std::map<std::string, Location *> &locations)
{
	std::string key;

	int autoindex = UNSET;
	std::string uri;
	std::string root = "";
	std::vector<std::string> indexes;
	std::vector<std::string> methods;
	std::map<std::string, std::string> redirections;
	size_t index = 0;

	while (!isspace(directive[index]) && directive[index] != '{')
		index++;
	uri = directive.substr(0, index);
	while (isspace(directive[index]))
		index++;
	index += sizeof('{');
	while (index < directive.size() && directive[index] != '}')
	{
		while (index < directive.size() && (isspace(directive[index]) || directive[index] == '}'))
			index++;
		if (index >= directive.size())
			break;
		key = getDirectiveKey(directive, index);
		if (key == "autoindex")
			parseAutoindex(getDirective(directive, index), autoindex);
		else if (key == "index")
			parseIndexes(getDirective(directive, index), indexes);
		else if (key == "root")
			parseRoot(getDirective(directive, index), root);
		else if (key == "methods")
			parseMethods(getDirective(directive, index), methods);
		else if(key == "redirection")
			parseRedirection(getDirective(directive, index), redirections);
		else
			throw std::invalid_argument(PARSING_UNEXPECTED);
		index++;
	}
	locations[uri] = new Location(autoindex, root, indexes, methods, redirections);
}

Server *getServer(std::string data)
{
    std::string key;

    int autoindex = true;
    ssize_t max_body_size = DEFAULT_MAX_BODY_SIZE;
    std::string root = "";
    std::vector<std::string> hosts;
    std::vector<unsigned int> ports;
    std::map<unsigned int, std::string> error_pages;
    std::map<std::string, Location *> locations;
    std::map<std::string, std::string> redirections;

    for (size_t index = 0; data[index]; (void)0)
    {
        while (index < data.size() && (isspace(data[index]) || data[index] == '}'))
            index++;
        if (index >= data.size())
            break;
        try
        {
            key = getDirectiveKey(data, index);
            if (key == "autoindex")
                parseAutoindex(getDirective(data, index), autoindex);
            else if (key == "client_max_body_size")
                parseMaxBodySize(getDirective(data, index), max_body_size);
            else if (key == "root")
                parseRoot(getDirective(data, index), root);
            else if (key == "server_name")
                parseHost(getDirective(data, index), hosts);
            else if (key == "listen")
                parsePort(getDirective(data, index), ports);
            else if (key == "error_page")
                parseErrorPage(getDirective(data, index), error_pages);
            else if (key == "location")
                parseLocation(getLocationBlock(data, index), locations);
            else if (key == "redirection")
                parseRedirection(getDirective(data, index), redirections);
            else
                throw std::invalid_argument(PARSING_UNEXPECTED);
            if (index < data.size())
                index++;
        }
        catch (const std::exception &e)
        {
            if (!locations.empty())
            {
                for (std::map<std::string, Location *>::iterator it = locations.begin();
                     it != locations.end(); it++)
                    delete (it->second);
            }
            throw std::invalid_argument(PARSING_UNEXPECTED);
        }
    }
    return (new Server(autoindex, max_body_size, root, hosts, ports,
                      error_pages, locations, redirections));
}

static bool curlyBracketsAreMatched(const std::string &content)
{
	size_t curly_brackets;

	curly_brackets = 0;
	for (size_t index = 0; index < content.size(); index++)
	{
		if (content[index] == '{')
			curly_brackets++;
		else if (content[index] == '}')
			curly_brackets--;
	}
	return (curly_brackets == 0);
}

void parseConfigurationFile(const std::string &file, std::vector<Server *> &servers)
{
	std::string content = getConfContent(file);

	if (curlyBracketsAreMatched(content) == false)
		throw std::invalid_argument(PARSING_UNMATCHED_BRACKET);
	for (size_t index = 0; index < content.size(); index++)
	{
		while (isspace(content[index]))
			index++;
		if (content.compare(index, SIZE_SERVER, SERVER_BLOCK_NAME) != 0)
			throw std::invalid_argument(PARSING_UNEXPECTED);
		servers.push_back(getServer(getServerBlock(content, index)));
	}
}
