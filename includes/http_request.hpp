#pragma once

#include "Server.hpp"

class Http_request
{
private:
    std::string method;
    std::string path;
    bool has_body;
    std::string ContentType;
    size_t contentLength;

public:

};