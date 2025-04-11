# include "Server.hpp"

Http_request::Http_request()
{

};

std::string Http_request::get_content_type() const
{
    return(_ContentType);
};

std::string Http_request::get_method() const
{
    return(_method);
};
std::string Http_request::get_uri() const
{
    return(_uri);
};

std::string Http_request::get_version() const
{
    return(_http_version);
};

bool Http_request::get_has_body() const
{
    return(_has_body);
};

size_t Http_request::get_content_length() const
{
    return(_contentLength);
};

std::map<std::string, std::string>Http_request::get_headers()const 
{
    return(_headers);
}

void Http_request::set_content_type(const std::string& type) 
{
    _ContentType = type;
}

void Http_request::set_method(const std::string& m) 
{
    _method = m;
}

void Http_request::set_uri(const std::string& u) 
{
    _uri = u;
}

void Http_request::set_version(const std::string& version) 
{
    _http_version = version;
}

void Http_request::set_has_body(bool has) 
{
    _has_body = has;
}

void Http_request::set_content_length(size_t length) 
{
    _contentLength = length;
}

void Http_request::set_headers(const std::map<std::string, std::string> headers)
{
    _headers = headers;
}

std::map<std::string, std::string> Http_request::parse_headers(std::string& request) 
{
    std::map<std::string, std::string> headers;
    
    size_t start = request.find("\r\n");
    if (start == std::string::npos) 
        return headers;
    start += 2;
    size_t end = request.find("\r\n\r\n", start);
    if (end == std::string::npos) 
        end = request.length();

    size_t line_start = start;
    while (line_start < end) 
    {
        size_t line_end = request.find("\r\n", line_start);
        if (line_end == std::string::npos || line_end > end)
            line_end = end;

        std::string line = request.substr(line_start, line_end - line_start);
        line_start = line_end + 2;

        size_t colon = line.find(": ");
        if (colon == std::string::npos) 
        {
            colon = line.find(":");
            if (colon == std::string::npos) 
                continue;
        }

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + (line[colon + 1] == ' ' ? 2 : 1));

        headers[key] = value;
    }

    // Boucle pour test
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it) 
    {
        std::cout << MAGENTA << "[" << it->first << "]" << CYAN << " " << it->second << RESET << std::endl;
    }
    std::cerr << std::endl;

    return headers;
}