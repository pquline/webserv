#include "Server.hpp"

static void saveMapToFile(const std::map<std::string, std::string> &data, const std::string &filepath, int eventFd)
{
    (void)eventFd;
    std::ofstream file(filepath.c_str());
    if (!file)
        return; // sendError(eventFd, )?

    for (std::map<std::string, std::string>::const_iterator it = data.begin(); it != data.end(); ++it)
    {
        file << it->first << ": " << it->second << "\n";
    }

    file.close();
}

static std::string getCookie(const std::string &request)
{
    size_t cookiePos = request.find("Cookie");
    size_t cookieEnd = request.find("\r\n", cookiePos);
    std::string cookie = request.substr(cookiePos + 18, cookieEnd - cookiePos - 18);
    return cookie;
}

void Server::handlePostRequest(int eventFd, std::string &request)
{
    HTTPRequest http_request;

    size_t header_end = request.find("\r\n\r\n");
    if (header_end == std::string::npos)
    {
        sendError(eventFd, 400, "Bad Request");
        return;
    }

    std::string first_line = request.substr(0, request.find("\r\n"));
    std::vector<std::string> request_splitted = ft_split(first_line, ' ');
    if (request_splitted.size() != 3)
    {
        sendError(eventFd, 400, "Bad Request");
        return;
    }

    if (request_splitted[2].compare(GOOD_HTTP_VERSION) != 0)
    {
        sendError(eventFd, 505, "HTTP Version Not Supported");
        return;
    }

    http_request.setVersion(request_splitted[2]);
    std::string uri = request_splitted[1];
    http_request.setURI(uri);
    http_request.setHeaders(http_request.parseHeaders(request));

    bool isCGI = false;
    size_t dot_pos = uri.find_last_of('.');
    if (dot_pos != std::string::npos)
    {
        std::string extension = uri.substr(dot_pos);
        if (extension == ".py" || extension == ".php" || extension == ".pl" || extension == ".sh" || extension == ".cgi")
            isCGI = true;
    }

    if (isCGI)
    {
        callCGI(eventFd, request);
        return;
    }

    size_t content_length = http_request.getContentLength();
    if (content_length == 0)
    {
        sendError(eventFd, 411, "Length Required");
        return;
    }

    if (content_length > (size_t)_max_body_size)
    {
        sendError(eventFd, 413, "Entity Too Large");
        return;
    }

    std::string body = request.substr(header_end + 4, content_length);
    std::string content_type = http_request.getContentType();

    if (content_type.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        std::map<std::string, std::string> form_data = parse_url_encoded(body);
        std::string test = form_data["data"];

        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "\r\n"
                               "Received data:" +
                               test;

        send(eventFd, response.c_str(), response.size(), 0);
    }
    else if (content_type.find("multipart/form-data") != std::string::npos)
    {
        size_t boundary_pos = content_type.find("boundary=");
        if (boundary_pos == std::string::npos)
        {
            sendError(eventFd, 400, "Bad Request: No boundary in Content-Type");
            return;
        }

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
            size_t part_header_end = part.find("\r\n\r\n");
            if (part_header_end == std::string::npos)
                continue;

            std::string headers = part.substr(0, part_header_end);
            std::string content = part.substr(part_header_end + 4);

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
            if (filename_pos != std::string::npos && headers[filename_pos + 10] != '\"')
            {
                std::string session_id = getCookie(request);
                if (session_id.empty())
                    session_id = "anonymous";
                filename_pos += 10;
                size_t filename_end = headers.find("\"", filename_pos);
                std::string original_filename = headers.substr(filename_pos, filename_end - filename_pos);
                std::string extension = ".bin";
                size_t ext_dot = original_filename.find_last_of('.');
                if (ext_dot != std::string::npos)
                    extension = original_filename.substr(ext_dot);
                std::string new_filename = session_id + extension;
                std::string upload_path = "www/" + new_filename;
                std::ofstream ofs(upload_path.c_str(), std::ios::binary);
                if (ofs)
                {
                    ofs.write(content.c_str(), static_cast<std::streamsize>(content.size()));
                    ofs.close();
                    saved_files.push_back(new_filename);
                    form_data[name] = new_filename;
                }
                else
                {
                    sendError(eventFd, 500, "Internal Server Error: Failed to save file");
                    return;
                }
            }
            else
            {
                form_data[name] = content;
            }
        }

        std::string cookie = getCookie(request);
        if (cookie.empty())
            cookie = "anonymous";

        std::string cookiePath = "www/" + cookie + ".txt";
        saveMapToFile(form_data, cookiePath, eventFd);

        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "\r\n"
                               "Received data:\n";

        std::map<std::string, std::string>::iterator it;
        for (it = form_data.begin(); it != form_data.end(); ++it)
            response += "- " + it->first + ": " + it->second + "\n";

        if (!saved_files.empty())
        {
            response += "Saved file:\n";
            for (size_t i = 0; i < saved_files.size(); ++i)
                response += "- " + saved_files[i] + "\n";
        }

        send(eventFd, response.c_str(), response.size(), 0);
    }
    else if (content_type.find("text/plain") != std::string::npos)
    {
        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/plain\r\n"
                               "\r\n"
                               "Received text data:\n" +
                               body;

        send(eventFd, response.c_str(), response.size(), 0);
    }
    else
    {
        sendError(eventFd, 415, "Unsupported Media Type");
    }
}

void Server::callCGI(int eventFd, std::string &request)
{
    std::string requestTarget = parseRequestTarget(request);
    std::string scriptPath = "www" + requestTarget;

    if (access(scriptPath.c_str(), F_OK) != 0)
        return sendError(eventFd, 404, "CGI Script Not Found");

    if (access(scriptPath.c_str(), X_OK) != 0)
        return sendError(eventFd, 403, "CGI Script Not Executable");

    std::string interpreter;
    size_t dot_pos = scriptPath.find_last_of('.');
    if (dot_pos != std::string::npos)
    {
        std::string extension = scriptPath.substr(dot_pos);
        if (extension == ".py")
            interpreter = "/usr/bin/python3";
        else if (extension == ".php")
            interpreter = "/usr/bin/php";
        else if (extension == ".pl")
            interpreter = "/usr/bin/perl";
        else if (extension == ".sh")
            interpreter = "/bin/bash";
        else
            interpreter = "";
    }
    else
        interpreter = "";

    int pipefd[2];
    if (pipe(pipefd) == -1)
        return sendError(eventFd, 500, "Internal Server Error (pipe)");

    pid_t pid = fork();
    if (pid < 0)
        return sendError(eventFd, 500, "Internal Server Error (fork)");

    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        std::string contentLength = getHeader(request, "Content-Length");
        std::string contentType = getHeader(request, "Content-Type");
        std::string queryString = "";

        size_t queryPos = requestTarget.find('?');
        if (queryPos != std::string::npos)
        {
            queryString = requestTarget.substr(queryPos + 1);
            requestTarget = requestTarget.substr(0, queryPos);
        }

        std::string cookie = getCookie(request);

        std::vector<std::string> env_strings;
        env_strings.push_back("REQUEST_METHOD=" + (request.find("GET") != std::string::npos ? std::string("GET") : std::string("POST")));
        env_strings.push_back("CONTENT_LENGTH=" + contentLength);
        env_strings.push_back("CONTENT_TYPE=" + contentType);
        env_strings.push_back("SCRIPT_NAME=" + requestTarget);
        env_strings.push_back("QUERY_STRING=" + queryString);
        env_strings.push_back("HTTP_COOKIE=" + cookie);
        std::cerr << cookie << std::endl;
        env_strings.push_back("PATH_INFO=");
        env_strings.push_back("PATH_TRANSLATED=");
        env_strings.push_back("REMOTE_ADDR=127.0.0.1");
        env_strings.push_back("REMOTE_HOST=localhost");
        env_strings.push_back("SERVER_NAME=localhost");
        env_strings.push_back("SERVER_PORT=8080");
        env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
        env_strings.push_back("SERVER_SOFTWARE=WebServ/1.0");

        std::vector<char *> envp;
        for (size_t i = 0; i < env_strings.size(); ++i)
            envp.push_back(const_cast<char *>(env_strings[i].c_str()));
        envp.push_back(NULL);

        std::vector<char *> argv;
        if (!interpreter.empty())
            argv.push_back(const_cast<char *>(interpreter.c_str()));

        argv.push_back(const_cast<char *>(scriptPath.c_str()));
        argv.push_back(NULL);

        if (interpreter.empty())
            execve(scriptPath.c_str(), &argv[0], &envp[0]);
        else
            execve(interpreter.c_str(), &argv[0], &envp[0]);

        exit(1);
    }
    else
    {
        close(pipefd[1]);

        std::string output;
        char buffer[4096];
        ssize_t n;

        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(pipefd[0], &readfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        while (true)
        {
            int ready = select(pipefd[0] + 1, &readfds, NULL, NULL, &tv);
            if (ready == -1)
            {
                // Error in select
                break;
            }
            else if (ready == 0)
            {
                // Timeout
                break;
            }
            else
            {
                // Data available
                n = read(pipefd[0], buffer, sizeof(buffer) - 1);
                if (n <= 0)
                {
                    // End of file or error
                    break;
                }
                buffer[n] = '\0';
                output.append(buffer, static_cast<unsigned long>(n));
            }
        }

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (!output.empty())
        {
            size_t header_end = output.find("\r\n\r\n");
            if (header_end != std::string::npos)
                send(eventFd, output.c_str(), output.length(), 0);
            else
            {
                std::ostringstream stream;
                stream << output.length();
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " +
                    stream.str() + "\r\n"
                                   "\r\n" +
                    output;
                send(eventFd, response.c_str(), response.length(), 0);
            }
        }
        else
            sendError(eventFd, 500, "CGI Output Error");
    }
}
static std::string generateSessionId()
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static bool seeded = false;
    if (!seeded)
    {
        srand(static_cast<unsigned int>(time(NULL)));
        seeded = true;
    }

    std::string id;
    id.reserve(32);
    for (int i = 0; i < 32; ++i)
    {
        id += alphanum[static_cast<unsigned long>(rand()) % (sizeof(alphanum) - 1)];
    }

    return id;
}

void Server::handleGetRequest(int eventFd, std::string &request)
{
    HTTPRequest http_request;

    std::string first_line = request.substr(0, request.find("\r\n"));
    std::vector<std::string> request_splitted = ft_split(first_line, ' ');
    if (request_splitted.size() != 3)
        return sendError(eventFd, 400, "Bad Request");
    if (request_splitted[2].compare(GOOD_HTTP_VERSION))
        return sendError(eventFd, 505, "HTTP Version Not Supported");
    http_request.setVersion(request_splitted[2]);
    std::string uri = request_splitted[1];

    const std::map<std::string, std::string> &redirections = getRedirections();
    if (redirections.find(uri) != redirections.end())
    {
        const std::string &destination = redirections.at(uri);
        std::string response = "HTTP/1.1 301 Moved Permanently\r\n"
                               "Location: " +
                               destination + "\r\n"
                                             "Content-Length: 0\r\n"
                                             "\r\n";
        send(eventFd, response.c_str(), response.size(), 0);
        return;
    }
    if (uri == "/")
    {
        uri = "/index.html"; 
    }
    if (uri[uri.length() - 1] == '/')
    {
    std::cerr << "[DEBUG AUTOINDEX]: " << _autoindex << std::endl;
    if (_autoindex)
    {
        std::string dir_path = "www" + uri;
        std::cerr << "[DEBUG]: " << dir_path << std::endl;
        
        DIR *dir;
        struct dirent *ent;
        
        if ((dir = opendir(dir_path.c_str())) != NULL)
        {
            std::string html = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + uri + "</title>\n</head>\n";
            html += "<body>\n<h1>Index of " + uri + "</h1>\n<hr>\n<ul>\n";
            
            if (uri != "/")
            {
                size_t last_slash = uri.substr(0, uri.length() - 1).find_last_of('/');
                std::string parent_path = uri.substr(0, last_slash + 1);
                html += "<li><a href=\"" + parent_path + "\">../</a></li>\n";
            }
            
            while ((ent = readdir(dir)) != NULL)
            {
                std::string name = ent->d_name;
                if (name == "." || name == "..") continue;
                
                std::string full_path = dir_path + name;
                struct stat statbuf;
                
                if (stat(full_path.c_str(), &statbuf) == 0)
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        name += "/";
                        html += "<li><a href=\"" + uri + name + "\">" + name + "</a></li>\n";
                    }
                    else
                    {
                        html += "<li><a href=\"" + uri + name + "\">" + name + "</a></li>\n";
                    }
                }
            }
            closedir(dir);
            
            html += "</ul>\n<hr>\n</body>\n</html>";
            
            std::ostringstream sizeStream;
            sizeStream << html.size();
            std::string sizeStr = sizeStream.str();
            
            std::string response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html\r\n"
                                  "Content-Length: " + sizeStr + "\r\n"
                                  "\r\n" + html;
            
            send(eventFd, response.c_str(), response.size(), 0);
            return;
        }
        else
        {
            return sendError(eventFd, 403, "Forbidden");
        }
    }
    else
    {
        return sendError(eventFd, 403, "Forbidden");
    }
    }
    http_request.setURI(uri);
    http_request.setHeaders(http_request.parseHeaders(request));

    std::string file_path = "www" + uri;

    std::ifstream file(file_path.c_str());
    if (!file.is_open())
        return sendError(eventFd, 404, "Page Not Found");

    bool isCGI = false;
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos != std::string::npos)
    {
        std::string extension = file_path.substr(dot_pos);
        if (extension == ".py" || extension == ".php" || extension == ".pl" || extension == ".sh" || extension == ".cgi")
            isCGI = true;
    }

    if (isCGI)
        callCGI(eventFd, request);
    else
    {
        std::stringstream buf;
        buf << file.rdbuf();
        file.close();

        std::string content = buf.str();
        std::string content_type = "text/html";

        if (dot_pos != std::string::npos)
        {
            std::string extension = file_path.substr(dot_pos);
            if (extension == ".css")
                content_type = "text/css";
            else if (extension == ".js")
                content_type = "application/javascript";
            else if (extension == ".png")
                content_type = "image/png";
            else if (extension == ".jpg" || extension == ".jpeg")
                content_type = "image/jpeg";
            else if (extension == ".gif")
                content_type = "image/gif";
            else if (extension == ".ico")
                content_type = "image/x-icon";
            else if (extension == ".txt")
                content_type = "text/plain";
        }

        std::ostringstream sizeStream;
        sizeStream << content.size();
        std::string sizeStr = sizeStream.str();

        size_t cookiePos = request.find("Cookie");
        std::string sessionID = "";
        std::string setCookie = "";
        if (cookiePos != std::string::npos)
        {
            sessionID = getCookie(request);
            std::cout << GREEN << "Welcome back my heroine!" << RESET << std::endl;
            std::cout << "SessionID: " << sessionID << std::endl;
        }
        else
        {
            sessionID = generateSessionId();
            setCookie = "Set-Cookie: SESSIONID=" + sessionID + "; Path=/; Max-Age=3600\r\n";
            _cookies[sessionID] = "firstname: Unknown\nlastname: Unknown\nphoto: \nschool: Unknown\n";
            std::string cookiePath = "www/" + sessionID + ".txt";
            std::ofstream cookieFile(cookiePath.c_str());
            if (!cookieFile)
            {
                std::cerr << RED << "Failed to create cookie file: " << cookiePath << RESET << std::endl;
            }
            cookieFile << _cookies[sessionID];
            cookieFile.close();

            std::cout << GREEN << "New Cookie generated!" << RESET << std::endl;
        }
        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: " +
                               content_type + "\r\n"
                                              "Content-Length: " +
                               sizeStr + "\r\n" + setCookie +
                               "\r\n" +
                               content;
        send(eventFd, response.c_str(), response.size(), 0);
    }
}

void Server::handleDeleteRequest(int eventFd, std::string &request)
{
    HTTPRequest http_request;

    std::string first_line = request.substr(0, request.find("\r\n"));
    std::vector<std::string> request_splitted = ft_split(first_line, ' ');
    if (request_splitted.size() != 3)
        return sendError(eventFd, 400, "Bad Request");
    if (request_splitted[2].compare(GOOD_HTTP_VERSION))
        return sendError(eventFd, 505, "HTTP Version Not Supported");

    std::string uri = request_splitted[1];
    std::string file_path = "www" + uri;

    if (access(file_path.c_str(), F_OK) != 0)
        return sendError(eventFd, 404, "Not Found");
    struct stat path_stat;
    if (stat(file_path.c_str(), &path_stat) != 0)
        return sendError(eventFd, 500, "Internal Server Error");
    if (S_ISDIR(path_stat.st_mode))
        return sendError(eventFd, 403, "Forbidden");
    if (remove(file_path.c_str()) != 0)
        return sendError(eventFd, 500, "Internal Server Error");
    std::string response = "HTTP/1.1 204 No Content\r\n"
                           "Content-Length: 0\r\n"
                           "\r\n";
    send(eventFd, response.c_str(), response.size(), 0);
}

void Server::parseRequest(int eventFd, ssize_t bytesRead, char *buffer)
{
    std::string request(buffer, static_cast<size_t>(bytesRead));

    std::string first_line = request.substr(0, request.find("\r\n"));
    if (first_line.find("GET") != std::string::npos)
        Server::handleGetRequest(eventFd, request);
    else if (first_line.find("POST") != std::string::npos)
        Server::handlePostRequest(eventFd, request);
    else if (first_line.find("DELETE") != std::string::npos)
        Server::handleDeleteRequest(eventFd, request);
    else
        sendError(eventFd, 400, "Bad Request");
    std::cerr << GRAY << buffer << RESET << std::endl;
};
