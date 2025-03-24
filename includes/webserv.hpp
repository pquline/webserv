#pragma once

#include <cerrno>
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
#include <map>
#include "Server.hpp"

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
# define RESET "\033[0m"

#define ERROR_PREFIX	"[Error]: "
#define ERROR_USAGE		"Usage: ./webserv [file.conf]"

#define DEFAULT_CONF	"webserv.conf"
#define CONF_EXTENSION	".conf"
#define SIZE_EXTENSION	5
