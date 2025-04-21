#include "Server.hpp"

void 	Server::handleGetRequest(int eventFd, std::string& request)
{
    Http_request http_request;

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
    if (uri == "/") {
        uri = "/index.html";  // Fichier par défaut
    }
    http_request.set_uri(uri);
    http_request.set_headers(http_request.parse_headers(request));
    // Check headers obligatoire
    // Check html
    std::string file_path = "pages" + uri;
    std::ifstream file(file_path.c_str());
    if (!file.is_open())
    {
        sendError(eventFd, 404, "Page Not Found");
		return ;
    }
    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    std::string html = buf.str();

    std::ostringstream sizeStream;
    sizeStream << html.size();
    std::string sizeStr = sizeStream.str();

    std::string response = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: " + sizeStr + "\r\n"
                      "\r\n" + html;
    // std::cerr << CYAN << response << RESET << std::endl;
    send(eventFd, response.c_str(), response.size(), 0);
}

void 	Server::handleDeleteRequest(int eventFd, std::string& request)
{
    (void)eventFd;
    (void)request;
}
