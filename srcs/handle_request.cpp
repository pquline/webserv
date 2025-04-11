#include "Server.hpp"

void 	Server::handleGetRequest(int eventFd, std::string& request)
{
    (void)eventFd;
    (void)request;

    Http_request http_request;

    std::string first_line= request.substr(0, request.find("\r\n"));
    std::vector<std::string> request_splitted = ft_split(first_line, ' ');
    if (request_splitted.size() != 3) 
    {
        // Bad request
        return;
    }
    if(request_splitted[2].compare(GOOD_HTTP_VERSION))
    {
        // Bad HTTP version
        return;
    }
    http_request.set_version(request_splitted[2]);
    http_request.set_uri(request_splitted[1]);
    http_request.set_headers(http_request.parse_headers(request));

}

void 	Server::handlePostRequest(int eventFd, std::string& request)
{
    (void)eventFd;
    (void)request;
}


void 	Server::handleDeleteRequest(int eventFd, std::string& request)
{
    (void)eventFd;
    (void)request;
}
