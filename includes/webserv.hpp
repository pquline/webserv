#pragma once

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

#define COLOR_RED		"\e[1\e[31m"
#define COLOR_GREEN		"\e[1\e[32m"
#define COLOR_YELLOW	"\e[1\e[33m"
#define COLOR_BLUE		"\e[1\e[34m"
#define COLOR_PURPLE	"\e[1\e[35m"
#define COLOR_NONE		"\e[0m"

#define ERROR_PREFIX	"[Error]: "
#define ERROR_USAGE		"Usage: ./webserv [file.conf]"

#define DEFAULT_CONF	"webserv.conf"
#define CONF_EXTENSION	".conf"
#define SIZE_EXTENSION	5
