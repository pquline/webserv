#include "Server.hpp"

void 	Server::handlePostRequest(int eventFd, const std::string& request)
{
    (void)eventFd;
    (void)request;
    std::cerr << GREEN << "prout POST" << RESET << std::endl;
}

void 	Server::handleGetRequest(int eventFd, const std::string& request)
{
    (void)eventFd;
    (void)request;
    std::cerr << YELLOW << "prout GET" << RESET << std::endl;    
}

void 	Server::handleDeleteRequest(int eventFd, const std::string& request)
{
    (void)eventFd;
    (void)request;
    std::cerr << GRAY << "prout DELETE" << RESET << std::endl;    
}

