#pragma once

class Http_request
{
public:
    Http_request();

private:
    std::string _method;
    std::string _uri;
    std::string _http_version;

    bool _has_body;
    std::string _ContentType;
    size_t _contentLength;

    std::map<std::string, std::string> _headers;

public:

    std::string get_method() const;
    std::string get_uri() const;
    std::string get_version() const;
    std::string get_content_type() const;
    bool get_has_body() const;
    size_t get_content_length() const;
    std::map<std::string, std::string> get_headers() const;

    void set_content_type(const std::string& type);
    void set_method(const std::string& m);
    void set_uri(const std::string& u);
    void set_version(const std::string& version);
    void set_has_body(bool has);
    void set_content_length(size_t length);
    void set_headers(const std::map<std::string, std::string> headers);

    std::map<std::string, std::string> parse_headers(std::string& request);
};