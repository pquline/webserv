#pragma once

#include <iostream>

class Response
{
    public:
        Response(std::string http_ver, int code, std::string phrase, std::string body);
    
    private:
        std::string	_http_version;
        int			_status_code;
        std::string	_reason_phrase;
        std::string	body;
};
