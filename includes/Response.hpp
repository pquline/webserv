#pragma once

#include "webserv.hpp"
#include "Cookie.hpp"
#include "Session.hpp"

class Response {

		int _statusCode;
		std::string _statusMessage;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::vector<Cookie> _cookies;
		Session* _session;

	public:
		Response();
		~Response();

		int getStatusCode() const { return _statusCode; }
		const std::string& getStatusMessage() const { return _statusMessage; }
		const std::map<std::string, std::string>& getHeaders() const { return _headers; }
		const std::string& getBody() const { return _body; }
		const std::vector<Cookie>& getCookies() const { return _cookies; }
		Session* getSession() const { return _session; }

		void setStatusCode(int code) { _statusCode = code; }
		void setStatusMessage(const std::string& message) { _statusMessage = message; }
		void setHeader(const std::string& key, const std::string& value) { _headers[key] = value; }
		void setBody(const std::string& body);
		void setSession(Session* session);
		void addCookie(const Cookie& cookie);
		void removeCookie(const std::string& name);

		std::string toString() const;
};
