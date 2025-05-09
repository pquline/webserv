#pragma once

#include "webserv.hpp"

class HTTPRequest
{
		bool _hasBody;
		std::string _method;
		std::string _uri;
		std::string _version;
		std::string _contentType;
		size_t _contentLength;
		std::map<std::string, std::string> _headers;

	public:
		HTTPRequest();
		std::string getMethod() const { return (_method); };
		std::string getURI() const { return (_uri); };
		std::string getVersion() const { return (_version); };
		std::string getContentType() const { return (_contentType); };
		bool getHasBody() const { return (_hasBody); };
		size_t getContentLength() const { return (_contentLength); };
		std::map<std::string, std::string> getHeaders() const { return (_headers); };

		void setContentType(const std::string &type) { _contentType = type; };
		void setMethod(const std::string &m) { _method = m; };
		void setURI(const std::string &u) { _uri = u; };
		void setVersion(const std::string &version) { _version = version; };
		void setHasBody(bool has) { _hasBody = has; };
		void setContentLength(size_t length) { _contentLength = length; };

		void setHeaders(const std::map<std::string, std::string>& headers);
		std::map<std::string, std::string> parseHeaders(const std::string &request);
};
