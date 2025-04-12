#include "response.hpp"

Response::Response(std::string http_ver, int code, std::string phrase, std::string body)
{
    _http_version = http_ver;
    _status_code = code;
    _reason_phrase = phrase;
    _body = body;
}
