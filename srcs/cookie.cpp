#include "Cookie.hpp"

Cookie::Cookie() : _expires(0), _secure(false), _httpOnly(false) {}

Cookie::Cookie(const std::string& name, const std::string& value)
    : _name(name), _value(value), _expires(0), _secure(false), _httpOnly(false) {}

Cookie::~Cookie() {}

std::string Cookie::toString() const {
    std::ostringstream oss;
    oss << _name << "=" << _value;

    if (!_domain.empty()) {
        oss << "; Domain=" << _domain;
    }
    if (!_path.empty()) {
        oss << "; Path=" << _path;
    }
    if (_expires > 0) {
        char buf[100];
        struct tm* timeinfo = gmtime(&_expires);
        strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
        oss << "; Expires=" << buf;
    }
    if (_secure) {
        oss << "; Secure";
    }
    if (_httpOnly) {
        oss << "; HttpOnly";
    }

    return oss.str();
}

Cookie Cookie::fromString(const std::string& cookieStr) {
    Cookie cookie;
    std::istringstream iss(cookieStr);
    std::string token;

    if (std::getline(iss, token, ';')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            cookie._name = token.substr(0, pos);
            cookie._value = token.substr(pos + 1);
        }
    }

    while (std::getline(iss, token, ';')) {
        token = token.substr(token.find_first_not_of(" "));
        size_t pos = token.find('=');
        std::string attr = token.substr(0, pos);
        std::string value = (pos != std::string::npos) ? token.substr(pos + 1) : "";

        if (attr == "Domain") {
            cookie._domain = value;
        } else if (attr == "Path") {
            cookie._path = value;
        } else if (attr == "Expires") {
            struct tm tm;
            strptime(value.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &tm);
            cookie._expires = mktime(&tm);
        } else if (attr == "Secure") {
            cookie._secure = true;
        } else if (attr == "HttpOnly") {
            cookie._httpOnly = true;
        }
    }

    return cookie;
}

std::string Cookie::generateSessionId() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(NULL)));
        seeded = true;
    }

    std::string id;
    id.reserve(32);
    for (int i = 0; i < 32; ++i) {
        id += alphanum[static_cast<unsigned long>(rand()) % (sizeof(alphanum) - 1)];
    }

    return id;
}
