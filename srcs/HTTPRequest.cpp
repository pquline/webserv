# include "Server.hpp"

HTTPRequest::HTTPRequest() {};

void HTTPRequest::setHeaders(const std::map<std::string, std::string> headers)
{
    Headers = headers;

    std::map<std::string, std::string>::const_iterator content_type_it = Headers.find("Content-Type");
    if (content_type_it != Headers.end())
        _contentType = content_type_it->second;
    else
        _contentType.clear();

    std::map<std::string, std::string>::const_iterator content_length_it = Headers.find("Content-Length");
    if (content_length_it != Headers.end()) {
        const char* str = content_length_it->second.c_str();
        char* end_ptr;
        errno = 0;
        unsigned long len = std::strtoul(str, &end_ptr, 10);

        if (errno == 0 && *end_ptr == '\0')
        {
            _contentLength = len;
            _has_body = (len > 0);
        }
        else
        {
            _contentLength = 0;
            _has_body = false;
        }
    }
    else
    {
        _contentLength = 0;
        _has_body = false;
    }
}

std::map<std::string, std::string> HTTPRequest::parseHeaders(std::string& request)
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
    // // Boucle pour test
    // for (std::map<std::string, std::string>::const_iterator it = headers.begin();
    //      it != headers.end(); ++it)
    // {
    //     std::cout << YELLOW << "[" << it->first << "]" << CYAN << " " << it->second << RESET << std::endl;
    // }
    // std::cerr << std::endl;

    return headers;
}
