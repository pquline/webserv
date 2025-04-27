#include "Server.hpp"

void Server::callCGI(int eventFd, std::string &request)
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
		char *argv[] = {(char *)"/usr/bin/python3", (char *)"./www/cgi-bin/whoami.py", NULL};

		std::string contentLength = getHeader(request, "Content-Length");
		std::string contentType = getHeader(request, "Content-Type");

		std::vector<std::string> env_strings;
		env_strings.push_back("REQUEST_METHOD=POST");
		env_strings.push_back("CONTENT_LENGTH=" + contentLength);
		env_strings.push_back("CONTENT_TYPE=" + contentType);
		env_strings.push_back("SCRIPT_NAME=" + requestTarget);

		// Now build char* array for execve
		std::vector<char*> envp;
		for (size_t i = 0; i < env_strings.size(); ++i)
			envp.push_back(const_cast<char*>(env_strings[i].c_str()));
		envp.push_back(NULL);

		execve("/usr/bin/python3", &argv[0], &envp[0]);
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
}

void 	Server::handleGetRequest(int eventFd, std::string& request)
{
	Http_request http_request;

	std::string first_line= request.substr(0, request.find("\r\n"));
	std::vector<std::string> request_splitted = ft_split(first_line, ' ');
	if (request_splitted.size() != 3)
		return sendError(eventFd, 400, "Bad Request");
	if(request_splitted[2].compare(GOOD_HTTP_VERSION))
		return sendError(eventFd, 505, "HTTP Version Not Supported");
	http_request.set_version(request_splitted[2]);
	std::string uri = request_splitted[1];
	if (uri == "/") {
		uri = "/index.html";  // Fichier par défaut
	}
	http_request.set_uri(uri);
	http_request.set_headers(http_request.parse_headers(request));
	// Check headers obligatoire
	// Check html
	std::string file_path = "www" + uri;
	std::ifstream file(file_path.c_str());
	if (!file.is_open())
		return sendError(eventFd, 404, "Page Not Found");
	std::string checkCGI = file_path.substr(4, 3);
	if (checkCGI == "cgi") callCGI(eventFd, request);
	else
	{
		std::stringstream buf;
		buf << file.rdbuf();
		file.close();

		std::string content = buf.str();
		std::string content_type = "text/html"; // Default content type

		// Determine content type based on file extension
		size_t dot_pos = file_path.find_last_of('.');
		if (dot_pos != std::string::npos) {
			std::string extension = file_path.substr(dot_pos);
			if (extension == ".css") {
				content_type = "text/css";
			}
			// Add more content types here as needed
		}

		std::ostringstream sizeStream;
		sizeStream << content.size();
		std::string sizeStr = sizeStream.str();

		std::string response = "HTTP/1.1 200 OK\r\n"
			"Content-Type: " + content_type + "\r\n"
			"Content-Length: " + sizeStr + "\r\n"
			"\r\n" + content;
		send(eventFd, response.c_str(), response.size(), 0);
	}
}

void 	Server::handleDeleteRequest(int eventFd, std::string& request)
{
	(void)eventFd;
	(void)request;
}
