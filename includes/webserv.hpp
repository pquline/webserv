#pragma once

#include <cerrno>
#include <csignal>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <exception>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <sys/epoll.h>
#include <map>
#include "Server.hpp"
#include "Location.hpp"

# define nullptr 0
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define CYAN "\033[36m"
# define GRAY "\033[90m"
# define BOLD "\033[1m"
# define UNDER "\033[4m"
# define BLINK "\033[5m"
# define ERASE = "\033[2K\r"
# define RESET "\e[0m"

#define ERROR_PREFIX	"[Error]: "
#define ERROR_USAGE		"Usage: ./webserv [file.conf]"
#define ERROR_OPEN		"Cannot open "

#define DEFAULT_CONF	"webserv.conf"
#define CONF_EXTENSION	".conf"
#define SIZE_EXTENSION	5

#define ROOT_PATH "index.html"
#define GOOD_HTTP_VERSION "HTTP/1.1"

std::vector<std::string> ft_split(const std::string& str, char delimiter);

#define PARSING_UNEXPECTED			"Found unexpected data while parsing configuration file"
#define PARSING_UNMATCHED_BRACKET	"Found unmatched bracket(s) while parsing configuration file"
#define PARSING_AUTOINDEX			"Found unexpected autoindex directive while parsing configuration file"
#define PARSING_BODY_SIZE			"Found unexpected client_max_body_size directive while parsing configuration file"
#define PARSING_PORTS				"Found unexpected port directive while parsing configuration file"
#define PARSING_ERROR_PAGE			"Found unexpected error_page directive while parsing configuration file"

#define SERVER_BLOCK_NAME	"server"

#define SIZE_SERVER		6
#define SIZE_LOCATION	8

#define UNSET					-1
#define DEFAULT_MAX_BODY_SIZE	1000000

class	Server;

void	check_conf_pathname(const std::string& pathname);
void	parse_conf_file(const std::string& file, std::vector<Server *> &servers);
