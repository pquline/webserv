#include "Response.hpp"

Response::Response(const std::string& http_ver, int code, const std::string& phrase, const std::string& body): m_http_version(http_ver), m_status_code(code), m_reason_phrase(phrase), m_body(body)
{
    /*_http_version = http_ver;
    _status_code = code;
    _reason_phrase = phrase;
    _body = body;*/
}
