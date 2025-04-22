#include "Server.hpp"
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // For access()
#include <cstdio>    // For remove()

void Server::handlePostRequest(int eventFd, std::string &request)
{
    (void)eventFd;
    Http_request http_request;

    size_t header_end = request.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return sendError(eventFd, 400, "Bad Request");

    std::string first_line = request.substr(0, request.find("\r\n"));
    std::vector<std::string> request_splitted = ft_split(first_line, ' ');
    if (request_splitted.size() != 3)
        return sendError(eventFd, 400, "Bad Request");

    if (request_splitted[2].compare(GOOD_HTTP_VERSION))
        return sendError(eventFd, 505, "HTTP Version Not Supported");
    http_request.set_version(request_splitted[2]);
    std::string uri = request_splitted[1];
    http_request.set_uri(uri);
    http_request.set_headers(http_request.parse_headers(request));

    size_t content_length = http_request.get_content_length();
    if (content_length == 0)
        return sendError(eventFd, 411, "Length Required");

    if (content_length > (size_t)_max_body_size)
        return sendError(eventFd, 413, "Entity Too Large");

    std::string body = request.substr(header_end + 4, content_length);
    std::string content_type = http_request.get_content_type();

    if (content_type.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        std::map<std::string, std::string> form_data = parse_url_encoded(body);
        std::string test = form_data["data"];

        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type:  text/plain\r\n"
                               "\r\n"
                               "Received data:" + test;
        send(eventFd, response.c_str(), response.size(), 0);
    }
    else if (content_type.find("multipart/form-data") != std::string::npos)
    {
        size_t boundary_pos = content_type.find("boundary=");
        if (boundary_pos == std::string::npos)
            return sendError(eventFd, 400, "Bad Request: No boundary in Content-Type");

        std::string boundary = "--" + content_type.substr(boundary_pos + 9);
        std::vector<std::string> parts;
        size_t start = 0;
        size_t end = body.find(boundary);

        while (end != std::string::npos)
        {
            std::string part = body.substr(start, end - start);
            if (!part.empty())
                parts.push_back(part);
            start = end + boundary.length() + 2;
            end = body.find(boundary, start);
        }

        std::map<std::string, std::string> form_data;
        std::vector<std::string> saved_files;

        for (size_t i = 0; i < parts.size(); ++i)
        {
            std::string part = parts[i];
            size_t header_end = part.find("\r\n\r\n");
            if (header_end == std::string::npos)
                continue;

            std::string headers = part.substr(0, header_end);
            std::string content = part.substr(header_end + 4);

            if (!content.empty() && content[content.length() - 1] == '\n')
                content.erase(content.length() - 1);
            if (!content.empty() && content[content.length() - 1] == '\r')
                content.erase(content.length() - 1);

            size_t name_pos = headers.find("name=\"");
            if (name_pos == std::string::npos)
                continue;

            name_pos += 6;
            size_t name_end = headers.find("\"", name_pos);
            std::string name = headers.substr(name_pos, name_end - name_pos);

            size_t filename_pos = headers.find("filename=\"");
            if (filename_pos != std::string::npos)
            {
                std::string upload_path = "www/pp.jpg";

                // Overwrite if file already exists
                if (access(upload_path.c_str(), F_OK) == 0)
                {
                    if (remove(upload_path.c_str()) != 0)
                        return sendError(eventFd, 500, "Internal Server Error: Failed to remove existing pp.jpg");
                }

                std::ofstream ofs(upload_path.c_str(), std::ios::binary);
                if (ofs)
                {
                    ofs.write(content.c_str(), static_cast<long>(content.size()));
                    ofs.close();
                    saved_files.push_back("pp.jpg");
                }
                else
                {
                    return sendError(eventFd, 500, "Internal Server Error: Failed to save pp.jpg");
                }
            }
            else
            {
                form_data[name] = content;
            }
        }

        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "\r\n"
                               "Received data:\n";

        for (std::map<std::string, std::string>::iterator it = form_data.begin(); it != form_data.end(); ++it)
            response += "- " + it->first + ": " + it->second + "\n";

        if (!saved_files.empty())
        {
            response += "Saved files:\n";
            for (size_t i = 0; i < saved_files.size(); ++i)
                response += "- " + saved_files[i] + "\n";
        }
        send(eventFd, response.c_str(), response.size(), 0);
    }
}
