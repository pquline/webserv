#pragma once

#include "webserv.hpp"

class Response
{
    public:
        Response(const std::string& http_ver, int code, const std::string& phrase, const std::string& body);

    private:
        std::string	m_http_version;
        int			m_status_code;
        std::string	m_reason_phrase;
        std::string	m_body;
};
