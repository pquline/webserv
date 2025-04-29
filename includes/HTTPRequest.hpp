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
		std::vector<Cookie> _cookies;

	public:
		HTTPRequest();
		std::string getMethod() const { return (_method); };
		std::string getURI() const { return (_uri); };
		std::string getVersion() const { return (_version); };
		std::string getContentType() const { return (_contentType); };
		bool getHasBody() const { return (_hasBody); };
		size_t getContentLength() const { return (_contentLength); };
		std::map<std::string, std::string> getHeaders() const { return (_headers); };
		Cookie getCookie(const std::string& name) const {
			for (std::vector<Cookie>::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it) {
				if (it->getName() == name) {
					return *it;
				}
			}
			return Cookie();
		}

		void setContentType(const std::string &type) { _contentType = type; };
		void setMethod(const std::string &m) { _method = m; };
		void setURI(const std::string &u) { _uri = u; };
		void setVersion(const std::string &version) { _version = version; };
		void setHasBody(bool has) { _hasBody = has; };
		void setContentLength(size_t length) { _contentLength = length; };

		void setHeaders(const std::map<std::string, std::string>& headers);
		void parseCookies(const std::string& cookieHeader);
};
