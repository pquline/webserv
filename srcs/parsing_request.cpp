#include "Server.hpp"

void Server::startParsing(int eventFd, ssize_t bytesRead, char *buffer)
{
    (void)eventFd;
    (void)bytesRead;
    std::string request(buffer, static_cast<size_t>(bytesRead));
    // std::cerr << MAGENTA << "Received request:\n" << buffer << RESET << std::endl;
    
    std::string first_line = request.substr(0, request.find("\r\n"));
    if(first_line.find("GET") != std::string::npos)
    {
        std::cerr << YELLOW << "GET received" << RESET << std::endl;    
        Server::handleGetRequest(eventFd, request);
    }
    else if(first_line.find("POST") != std::string::npos)
    {
        std::cerr << GREEN << "POST received" << RESET << std::endl;
        Server::handlePostRequest(eventFd, request);
    }
    else if(first_line.find("DELETE") != std::string::npos)
    {
        std::cerr << GRAY << "DELETE received" << RESET << std::endl;    
        Server::handleDeleteRequest(eventFd, request);
    }
    else
    {
        // throw 400 bad_request
    }
};