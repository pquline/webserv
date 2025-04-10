#include "Server.hpp"

void Server::startParsing(int eventFd, ssize_t bytesRead, char *buffer)
{
    (void)eventFd;
    (void)bytesRead;
    std::string request(buffer, static_cast<size_t>(bytesRead));
    std::cerr << MAGENTA << "Start parsing received request:\n" << buffer << RESET << std::endl;
    
    std::cerr << RED << "first line:" << request.substr(0, request.find("\r\n")) << RESET << std::endl;
    if(request.find("GET"))
        Server::handleGetRequest(eventFd, request);
    else if(request.find("POST"))
        Server::handlePostRequest(eventFd, request);
    else if(request.find("DELETE"))
        Server::handleDeleteRequest(eventFd, request);
    else
    {
        // throw 400 bad_request
    }
};