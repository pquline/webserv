#include "webserv.hpp"

void logWithTimestamp(const std::string &message, const std::string &color)
{
	time_t now = time(NULL);
	struct tm *timeinfo = localtime(&now);
	char timeStr[9];
	strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);

	std::cerr << color << BOLD << "[" << timeStr << "] " << RESET;
	std::cerr << message << RESET << std::endl;
}

std::vector<std::string> ft_split(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}

	tokens.push_back(str.substr(start));
	return tokens;
}

std::string url_decode(const std::string &str)
{
	std::string result;
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (str[i] == '+')
			result += ' ';
		else if (str[i] == '%' && i + 2 < str.length())
		{
			std::string hex = str.substr(i + 1, 2);
			char ch = static_cast<char>(strtol(hex.c_str(), NULL, 16));
			result += ch;
			i += 2;
		}
		else
			result += str[i];
	}
	return result;
}

std::map<std::string, std::string> parse_url_encoded(const std::string &body)
{
	std::map<std::string, std::string> form_data;
	size_t pos = 0;

	while (pos < body.length())
	{
		size_t amp_pos = body.find('&', pos);
		if (amp_pos == std::string::npos)
			amp_pos = body.length();

		std::string pair = body.substr(pos, amp_pos - pos);
		size_t equal_pos = pair.find('=');

		if (equal_pos != std::string::npos)
		{
			std::string key = pair.substr(0, equal_pos);
			std::string value = pair.substr(equal_pos + 1);

			std::string decoded_key = url_decode(key);
			std::string decoded_value = url_decode(value);

			form_data[decoded_key] = decoded_value;
		}

		pos = amp_pos + 1;
	}

	return form_data;
}
