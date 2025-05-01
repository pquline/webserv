#include "Server.hpp"

Server::Server(int autoindex, ssize_t max_body_size, std::string root, std::vector<std::string> hosts, std::vector<unsigned int> ports, std::map<unsigned int, std::string> error_pages, std::map<std::string, Location *> locations, std::map<std::string, std::string> redirections) : _autoindex(autoindex), _max_body_size(max_body_size), _root(root), _hosts(hosts), _ports(ports), _error_pages(error_pages), _locations(locations), _redirections(redirections)
{
    if (_max_body_size == UNSET)
        throw std::invalid_argument(PARSING_UNEXPECTED);
    if (_root.empty())
        throw std::invalid_argument(PARSING_UNEXPECTED);
    if (_hosts.empty())
        _hosts.push_back("localhost");
    if (_ports.empty())
        _ports.push_back(8080);
}

Server::~Server()
{
	for (size_t i = 0; i < _serverFds.size(); i++)
		close(_serverFds[i]);
	if (!_locations.empty())
	{
		for (std::map<std::string, Location *>::iterator it = _locations.begin();
				it != _locations.end(); it++)
			delete (it->second);
	}
	std::cout << "Server shut down." << std::endl;
}

void Server::init()
{
	for (size_t i = 0; i < _ports.size(); ++i) {
		int serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (serverFd == -1)
			throw std::runtime_error("Socket creation failed");

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY); 
		addr.sin_port = htons(static_cast<uint16_t>(_ports[i]));

		if (bind(serverFd, (struct sockaddr *)(&addr), sizeof(addr)) < 0)
			throw std::runtime_error("Binding failed");

		if (listen(serverFd, 10) < 0)
			throw std::runtime_error("Listening failed");

		_serverFds.push_back(serverFd);
		std::cout << GREEN << "http://localhost:" << _ports[i] << "..." << RESET << std::endl;
	}
}

static std::string generateErrorPage(int code, const std::string &message)
{
	std::ostringstream page;
	page << "<!DOCTYPE html>\n"
		<< "<html lang=\"en\">\n"
		<< "<head>\n"
		<< "    <meta charset=\"UTF-8\">\n"
		<< "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		<< "    <title>Error " << code << "</title>\n"
		<< "    <link rel=\"stylesheet\" href=\"styles.css\">\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "    <div class=\"container\">\n"
		<< "        <h1>" << code << "</h1>\n"
		<< "        <p class=\"error-message\">" << message << "</p>\n"
		<< "        <div class=\"button-container-2\">\n"
		<< "            <a class=\"button\" href=\"/index.html\">Back to Home</a>\n"
		<< "        </div>\n"
		<< "    </div>\n"
		<< "</body>\n"
		<< "</html>\n";

	return page.str();
}

void Server::sendError(int fd, int code, const std::string &message)
{
	std::map<unsigned int, std::string>::const_iterator it = _error_pages.find(static_cast<unsigned int>(code));
	std::string body;
	if (it != _error_pages.end())
	{
		const std::string &error_page_path = it->second;
		std::ifstream error_file(error_page_path.c_str());
		if (!error_file)
		{
		}
		if (error_file.is_open())
		{
			std::ostringstream buffer;
			buffer << error_file.rdbuf();
			body = buffer.str();
			error_file.close();
		}
		else
			body = generateErrorPage(code, message);
	}
	else
		body = generateErrorPage(code, message);
	std::ostringstream response;

	std::cerr << BLUE ERROR_PREFIX << code << " " << message << RESET << std::endl;
	response << "HTTP/1.1 " << code << " " << message << "\r\n"
		<< "Content-Type: text/html\r\n"
		<< "Content-Length: " << body.size() << "\r\n"
		<< "\r\n"
		<< body;
	send(fd, response.str().c_str(), response.str().size(), 0);
}

void Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("fcntl F_GETFL");

	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl F_SETFL");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl F_SETFL");
}

int Server::get_autoindex(void) const
{
	return (_autoindex);
}

std::string Server::parseRequestTarget(const std::string &request)
{
	std::istringstream stream(request);
	std::string method, path;
	stream >> method >> path;
	return path;
}

std::string Server::getHeader(const std::string &request, const std::string &key)
{
	size_t pos = request.find(key + ": ");
	if (pos == std::string::npos)
		return "";
	size_t start = pos + key.length() + 2;
	size_t end = request.find("\r\n", start);
	return request.substr(start, end - start);
}

const std::map<std::string, std::string> &Server::getRedirections() const
{
	return _redirections;
}

const std::map<unsigned int, std::string> &Server::getErrorPages() const
{
	return _error_pages;
}

const std::vector<unsigned int> Server::getServerFds() const
{
	return _serverFds;
}
