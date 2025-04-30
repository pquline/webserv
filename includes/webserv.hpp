#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <climits>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <netinet/in.h>
#include <vector>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sstream>
#include <exception>
#include <list>
#include <set>
#include <map>
#include "Server.hpp"
#include "Location.hpp"

#define nullptr 0
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define GRAY "\033[90m"
#define BOLD "\033[1m"
#define UNDER "\033[4m"
#define BLINK "\033[5m"
#define ERASE = "\033[2K\r"
#define RESET "\e[0m"

#define DEBUG_PREFIX "[DEBUG]: "
#define ERROR_PREFIX "[Error]: "
#define ERROR_USAGE "Usage: ./webserv [file.conf]"
#define ERROR_OPEN "Cannot open "
#define DEFAULT_CONF "webserv.conf"
#define CONF_EXTENSION ".conf"
#define SIZE_EXTENSION 5
#define ROOT_PATH "index.html"
#define GOOD_HTTP_VERSION "HTTP/1.1"

#define PARSING_UNEXPECTED "Found unexpected data while parsing configuration file"
#define PARSING_UNMATCHED_BRACKET "Found unmatched bracket(s) while parsing configuration file"
#define PARSING_AUTOINDEX "Found unexpected autoindex directive while parsing configuration file"
#define PARSING_BODY_SIZE "Found unexpected client_max_body_size directive while parsing configuration file"
#define PARSING_PORTS "Found unexpected port directive while parsing configuration file"
#define PARSING_ERROR_PAGE "Found unexpected error_page directive while parsing configuration file"

#define SERVER_BLOCK_NAME "server"

#define SIZE_SERVER 6
#define SIZE_LOCATION 8
#define UNSET -1
#define DEFAULT_MAX_BODY_SIZE 1000000

class Server;

void checkConfPathname(const std::string &pathname);
void parseConfigurationFile(const std::string &file, std::vector<Server *> &servers);
std::string url_decode(const std::string &str);
std::map<std::string, std::string> parse_url_encoded(const std::string &body);
std::vector<std::string> ft_split(const std::string &str, char delimiter);
