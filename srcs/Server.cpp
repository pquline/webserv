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
}

void Server::init()
{
    for (size_t i = 0; i < _ports.size(); ++i)
    {
        int serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (serverFd == -1)
            throw std::runtime_error("Socket creation failed");

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(static_cast<uint16_t>(_ports[i]));

        if (bind(serverFd, (struct sockaddr *)(&addr), sizeof(addr)) < 0)
		{
			close(serverFd);
            throw std::runtime_error("Binding failed");
		}

        if (listen(serverFd, 10) < 0)
		{
			close(serverFd);
            throw std::runtime_error("Listening failed");
		}

        _serverFds.push_back(serverFd);
        std::ostringstream oss;
        oss << _ports[i];
        logWithTimestamp("http://localhost:" + oss.str(), GREEN);

        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        char timeStr[9];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);
        std::cerr << GREEN << BOLD << "[" << timeStr << "] " << RESET;
        std::cout << "Server name(s): ";
        for (std::vector<std::string>::const_iterator it = _hosts.begin(); it != _hosts.end(); it++)
            std::cout << "[" YELLOW << *it << RESET "] ";
        std::cout << std::endl;
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
         << "    <link rel=\"stylesheet\" href=\"/styles.css\">\n"
         << "</head>\n"
         << "<body>\n"
         << "    <div class=\"container\">\n"
         << "        <h1>" << code << "</h1>\n"
         << "        <p class=\"error-message\">" << message << "</p>\n"
         << "        <div class=\"button-container-2\">\n"
         << "            <a class=\"button\" href=\"/index.html\">Back</a>\n"
         << "        </div>\n"
         << "    </div>\n"
         << "</body>\n"
         << "</html>\n";

    return page.str();
}

std::string Server::sendError(int code, const std::string &message)
{
    std::map<unsigned int, std::string>::const_iterator it = _error_pages.find(static_cast<unsigned int>(code));
    std::string body;
    if (it != _error_pages.end())
    {
        const std::string &error_page_path = it->second;
        std::ifstream error_file(error_page_path.c_str());
        if (error_file.is_open())
        {
            std::ostringstream buffer;
            buffer << error_file.rdbuf();
            body = buffer.str();
            error_file.close();
        }
        else
        {
            body = generateErrorPage(code, message);
        }
    }
    else
    {
        body = generateErrorPage(code, message);
    }
    std::ostringstream response;

    std::ostringstream errorMsg;
    errorMsg << "Error " << code << ": " << message;
    logWithTimestamp(errorMsg.str(), RED);

    response << "HTTP/1.1 " << code << " " << message << "\r\n"
             << "Content-Type: text/html\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n"
             << body;

    return (response.str().c_str());
}

void Server::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("fcntl F_GETFL");
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

const std::vector<int> Server::getServerFds() const
{
    return _serverFds;
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

Location *Server::getExactLocation(const std::string &uri) const
{
    std::string normalizedUri = uri;

    if (normalizedUri.size() > 1 && *normalizedUri.rbegin() == '/')
    {
        normalizedUri.erase(normalizedUri.length() - 1);
    }

    std::map<std::string, Location *>::const_iterator it = _locations.find(normalizedUri);
    if (it != _locations.end())
    {
        return it->second;
    }

    size_t pos = normalizedUri.length();
    while (true)
    {
        pos = normalizedUri.rfind('/', pos);
        if (pos == std::string::npos)
            break;

        std::string parentUri = normalizedUri.substr(0, pos);
        if (parentUri.empty())
            parentUri = "/";

        it = _locations.find(parentUri);
        if (it != _locations.end())
        {
            return it->second;
        }

        if (pos == 0)
            break;
        pos--;
    }

    it = _locations.find("/");
    if (it != _locations.end())
    {
        return it->second;
    }

    return NULL;
}

std::string   Server::sendRedirection(const std::string &destination)
{
    std::string response = "HTTP/1.1 301 Moved Permanently\r\n"
                           "Location: " +
                           destination + "\r\n"
                                         "Content-Length: 0\r\n"
                                         "\r\n";
    return (response.c_str());
}
