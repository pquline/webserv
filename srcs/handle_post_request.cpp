#include "Server.hpp"

static void saveMapToFile(const std::map<std::string, std::string>& data, const std::string& filepath, int eventFd){
	(void)eventFd;
    std::ofstream file(filepath.c_str());
    if (!file) return; // sendError(eventFd, )?

    for (std::map<std::string, std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
        file << it->first << ": " << it->second << "\n";
    }

    file.close();
}

void Server::handlePostRequest(int eventFd, std::string &request)
{
    (void)eventFd;
    HTTPRequest http_request;

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
            if (filename_pos != std::string::npos && headers[filename_pos + 10] != '\"')
            {
                std::string upload_path = "www/default.png";

                // Overwrite if file already exists
                if (access(upload_path.c_str(), F_OK) == 0)
                {
                    if (remove(upload_path.c_str()) != 0)
                        return sendError(eventFd, 500, "Internal Server Error: Failed to remove existing default.png");
                }

                std::ofstream ofs(upload_path.c_str(), std::ios::binary);
                if (ofs)
                {
                    ofs.write(content.c_str(), static_cast<long>(content.size()));
                    ofs.close();
                    saved_files.push_back("default.png");
                }
                else
                {
                    return sendError(eventFd, 500, "Internal Server Error: Failed to save default.png");
                }
            }
            else
            {
                form_data[name] = content;
            }
        }

		saveMapToFile(form_data, "www/usrInfo.txt", eventFd);
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


/*
void Server::handlePostRequest(int eventFd, std::string &request)
{
	std::string requestTarget = parseRequestTarget(request); // e.g. "/cgi-bin/updateProfile.py"
	//std::string scriptPath = "." + requestTarget; // Assuming scripts are in ./cgi-bin/

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

		//char *argv[] = {(char *)"/usr/bin/python3", (char *)scriptPath.c_str(), NULL};
		char *argv[] = {(char *)"/usr/bin/python3", (char *)"./www/cgi-bin/updateProfile.py", NULL};

		std::string contentLength = getHeader(request, "Content-Length");
		std::string contentType = getHeader(request, "Content-Type");

		std::string method = "POST";
		char *envp[] = {
			(char *)("REQUEST_METHOD=" + method).c_str(),
			(char *)("CONTENT_LENGTH=" + contentLength).c_str(),
			(char *)("CONTENT_TYPE=" + contentType).c_str(),
			(char *)("SCRIPT_NAME=" + requestTarget).c_str(),
			(char *)"GATEWAY_INTERFACE=CGI/1.1",
			(char *)"SERVER_PROTOCOL=HTTP/1.1",
			NULL
		};

		execve("/usr/bin/python3", argv, envp);
		// error handle
	}
	else
	{
		close(pipefd[1]);
		char buffer[8192] = {0};
		ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
		close(pipefd[0]);

		if (n > 0)
		{
			std::ostringstream stream;
			stream << strlen(buffer);
			std::string response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + stream.str() + "\r\n"
				"\r\n" + std::string(buffer);
			send(eventFd, response.c_str(), response.length(), 0);
		}
		else
		{
			sendError(eventFd, 500, "CGI Output Error");
		}

		//waitpid(pid, NULL, 0); // Wait for CGI script to finish
	}
}*/
