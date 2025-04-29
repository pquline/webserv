#pragma once

class Cookie {
	private:
		std::string _name;
		std::string _value;
		std::string _domain;
		std::string _path;
		time_t _expires;
		bool _secure;
		bool _httpOnly;

	public:
		Cookie();
		Cookie(const std::string& name, const std::string& value);
		~Cookie();

		std::string getName() const { return _name; }
		std::string getValue() const { return _value; }
		std::string getDomain() const { return _domain; }
		std::string getPath() const { return _path; }
		time_t getExpires() const { return _expires; }
		bool isSecure() const { return _secure; }
		bool isHttpOnly() const { return _httpOnly; }

		void setDomain(const std::string& domain) { _domain = domain; }
		void setPath(const std::string& path) { _path = path; }
		void setExpires(time_t expires) { _expires = expires; }
		void setSecure(bool secure) { _secure = secure; }
		void setHttpOnly(bool httpOnly) { _httpOnly = httpOnly; }

		std::string toString() const;
		static Cookie fromString(const std::string& cookieStr);
		static std::string generateSessionId();
};
