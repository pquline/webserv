#include "Server.hpp"

volatile sig_atomic_t g_sig = 0;

static void handle_sigint(int signal)
{
    if (signal == SIGINT)
        g_sig = 1;
}

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
    memset(&m_address, 0, sizeof(m_address));
}

Server::~Server()
{
    close(m_serverFd);
    if (!_locations.empty())
    {
        for (std::map<std::string, Location *>::iterator it = _locations.begin();
             it != _locations.end(); it++)
            delete (it->second);
    }
    logWithTimestamp("Server shut down", GREEN);
}

void Server::init()
{
    m_serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverFd == -1)
        handleError("Socket creation failed");
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = htonl(INADDR_ANY);
    m_address.sin_port = htons(static_cast<uint16_t>(*_ports.begin()));

    if (bind(m_serverFd, (struct sockaddr *)(&m_address), sizeof(m_address)) < 0)
        handleError("Binding failed");

    if (listen(m_serverFd, 10) < 0)
        handleError("Listening failed");
    std::cout << INIT_PREFIX "Server name(s): ";
    for (std::vector<std::string>::const_iterator it = _hosts.begin(); it != _hosts.end(); it++)
        std::cout << "[" YELLOW << *it << RESET "] ";
    std::cout << std::endl;
    std::ostringstream oss;
    oss << *(_ports.begin());
    logWithTimestamp("http://localhost:" + oss.str(), GREEN);
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

void Server::run()
{
    signal(SIGINT, handle_sigint);
    int epollFd = epoll_create1(0);
    if (epollFd < 0)
        handleError("epoll_create1 failed");

    struct epoll_event ev, events[MAX_EVENT];
    ev.events = EPOLLIN;
    ev.data.fd = m_serverFd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, m_serverFd, &ev) < 0)
        handleError("epoll_ctl: server socket");

    std::map<int, std::string> requestBuffer;

    while (true)
    {
        int nfds = epoll_wait(epollFd, events, MAX_EVENT, 1000);
        if (nfds < 0)
        {
            if (g_sig)
            {
                std::cout << std::endl;
                logWithTimestamp("SIGINT received, servers shutting down...", RED);
                break;
            }
            else
            {
                handleError("epoll_wait");
            }
        }

        for (int i = 0; i < nfds; i++)
        {
            int eventFd = events[i].data.fd;

            if (eventFd == m_serverFd)
            {
                m_addressLen = sizeof(m_address);
                int connFd = accept(m_serverFd, (struct sockaddr *)&m_address, &m_addressLen);
                if (connFd < 0)
                    handleError("accept");

                setNonBlocking(connFd);
                ev.events = EPOLLIN;
                ev.data.fd = connFd;

                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, &ev) < 0)
                    handleError("epoll_ctl: client socket");
            }
            else
            {
                char buffer[BUFFER_SIZE] = {0};
                ssize_t bytesRead = read(eventFd, buffer, BUFFER_SIZE);

                if (bytesRead <= 0)
                {
                    requestBuffer.erase(eventFd);
                    close(eventFd);
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
                    continue;
                }

                requestBuffer[eventFd] += std::string(buffer, static_cast<size_t>(bytesRead));

                size_t bodySize = 0;
                if (isCompleteRequest(requestBuffer[eventFd], bodySize))
                {
                    Server::parseRequest(eventFd, requestBuffer[eventFd]);
                    requestBuffer.erase(eventFd);
                    close(eventFd);
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, eventFd, NULL);
                }
            }
        }
    }

    close(epollFd);
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

void Server::sendError(int fd, int code, const std::string &message)
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
            std::ostringstream debugMsg;
            debugMsg << "Error page file not found: " << error_page_path;
            logWithTimestamp(debugMsg.str(), RED);
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
    send(fd, response.str().c_str(), response.str().size(), 0);
}

void Server::handleError(const std::string &msg)
{
    logWithTimestamp(msg, RED);
    if (m_serverFd != -1)
        close(m_serverFd);
    exit(EXIT_FAILURE);
}

void Server::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        handleError("fcntl F_GETFL");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        handleError("fcntl F_SETFL");
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
