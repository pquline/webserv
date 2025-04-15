#include "Server.hpp"

std::string url_decode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '+') {
            result += ' ';
        } else if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            char ch = static_cast<char>(strtol(hex.c_str(), NULL, 16));
            result += ch;
            i += 2;
        } else {
            result += str[i];
        }
    }
    return result;
}

std::map<std::string, std::string> parse_url_encoded(const std::string& body) {
    std::map<std::string, std::string> form_data;
    size_t pos = 0;
    
    while (pos < body.length()) {
        size_t amp_pos = body.find('&', pos);
        if (amp_pos == std::string::npos) amp_pos = body.length();
        
        std::string pair = body.substr(pos, amp_pos - pos);
        size_t equal_pos = pair.find('=');
        
        if (equal_pos != std::string::npos) {
            std::string key = pair.substr(0, equal_pos);
            std::string value = pair.substr(equal_pos + 1);
            
            // URL decoding basique
            std::string decoded_key = url_decode(key);
            std::string decoded_value = url_decode(value);
            
            form_data[decoded_key] = decoded_value;
        }
        
        pos = amp_pos + 1;
    }
    
    return form_data;
}

void Server::handlePostRequest(int eventFd, std::string& request)
{
    (void)eventFd;
    (void)request;
    Http_request http_request;

    std::cerr << GREEN << request <<  RESET << std::endl;
    size_t header_end = request.find("\r\n\r\n");
    if (header_end == std::string::npos) 
    {
        // 400 Bad request
        return;
    }
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
    std::string uri = request_splitted[1];

    http_request.set_uri(uri);
    http_request.set_headers(http_request.parse_headers(request));
    size_t content_length = http_request.get_content_length();
    if(content_length == 0)
    {
        // Error 411 Length
    }
    std::string body = request.substr(header_end + 4, content_length);
    std::cerr << RED << http_request.get_content_type() << RESET << std::endl;
    if(http_request.get_content_type().compare("application/x-www-form-urlencoded") == 0)
    {
        
        std::map<std::string, std::string> form_data = parse_url_encoded(body);
        
        std::string test = form_data["data"];
        
        std::string response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type:  text/plain\r\n"
                              "\r\n"
                              "Donnees recues=" + test;
        send(eventFd, response.c_str(), response.size(), 0);
    }
}