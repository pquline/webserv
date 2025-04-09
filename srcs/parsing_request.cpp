#include "Server.hpp"

void Server::startParsing(int eventFd, ssize_t bytesRead, char *buffer)
{
    (void)eventFd;
    (void)bytesRead;
    std::string request(buffer, static_cast<size_t>(bytesRead));
    std::cerr << MAGENTA << "Start parsing received request:\n" << buffer << RESET << std::endl;
    
    std::cerr << RED << "first line: " << request.substr(0, request.find("\r\n")) << RESET << std::endl;
};