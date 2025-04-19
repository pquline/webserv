#include "Server.hpp"

void Server::handlePostRequest(int eventFd, std::string& request)
{
    (void)eventFd;
    (void)request;
    Http_request http_request;

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
    std::string content_type = http_request.get_content_type();
    if(content_type.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        
        std::map<std::string, std::string> form_data = parse_url_encoded(body);
        
        std::string test = form_data["data"];
        
        std::string response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type:  text/plain\r\n"
                              "\r\n"
                              "Donnees recues=" + test;
        send(eventFd, response.c_str(), response.size(), 0);
    }
    else if (content_type.find("multipart/form-data") != std::string::npos)
    {
    size_t boundary_pos = content_type.find("boundary=");
    if (boundary_pos == std::string::npos)
    {
        // "Bad Request: No boundary in Content-Type";
        return;
    }
    std::string boundary = content_type.substr(boundary_pos + 9);
    boundary = "--" + boundary;

    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = body.find(boundary);
    
    while (end != std::string::npos)
    {
        std::string part = body.substr(start, end - start);
        if (!part.empty())
        {
            parts.push_back(part);
        }
        start = end + boundary.length() + 2;
        end = body.find(boundary, start);
    }

    std::map<std::string, std::string> form_data;
    
    for (size_t i = 0; i < parts.size(); ++i)
    {
        std::string part = parts[i];
        
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos)
        {
            continue;
        }
        
        std::string headers = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4);
        
        size_t name_pos = headers.find("name=\"");
        if (name_pos == std::string::npos)
        {
            continue; 
        }
        name_pos += 6;
        size_t name_end = headers.find("\"", name_pos);
        std::string name = headers.substr(name_pos, name_end - name_pos);
        
        if (!content.empty() && content[content.length() - 1] == '\n')
        {
            content.erase(content.length() - 1);
        }
        if (!content.empty() && content[content.length() - 1] == '\r')
        {
            content.erase(content.length() - 1);
        }
        
        form_data[name] = content;
    }

    std::string nom = form_data["nom"];
    std::string prenom = form_data["prenom"];
    std::string age = form_data["age"];
    std::string school = form_data["school"];
    std::string birthday = form_data["birthday"];

    std::string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "Donnees recues:\n"
                          "- Nom: " + nom + "\n"
                          "- Prenom: " + prenom + "\n"
                          "- Age: " + age + "\n"
                          "- School: " + school + "\n"
                          "- Birthday: " + birthday;
    
    send(eventFd, response.c_str(), response.size(), 0);
    }
}