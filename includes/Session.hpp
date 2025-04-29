#pragma once

#include "Cookie.hpp"
#include "UserData.hpp"

class Session {

		std::string _id;
		UserData* _userData;
		time_t _created;
		time_t _lastAccessed;
		time_t _expires;
		bool _isNew;
		static std::map<std::string, std::string> _emptyMap;
		static std::map<std::string, Session*> _sessions;

	public:
		Session();
		Session(const std::string& id);
		~Session();

		const std::string& getId() const { return _id; }
		std::string getAttribute(const std::string& name) const {
			return _userData ? _userData->getAttribute(name) : "";
		}
		time_t getCreated() const { return _created; }
		time_t getLastAccessed() const { return _lastAccessed; }
		time_t getExpires() const { return _expires; }
		bool isNew() const { return _isNew; }
		bool isValid() const { return time(NULL) < _expires; }
		UserData* getUserData() const { return _userData; }

		void setAttribute(const std::string& name, const std::string& value) {
			if (_userData) {
				_userData->setAttribute(name, value);
				refresh();
			}
		}
		void removeAttribute(const std::string& name) {
			if (_userData) {
				_userData->removeAttribute(name);
				refresh();
			}
		}
		void setExpires(time_t expires) { _expires = expires; }
		void setNew(bool isNew) { _isNew = isNew; }

		void invalidate();
		void refresh();
		Cookie createSessionCookie() const;

		const std::map<std::string, std::string>& getAttributes() const {
			return _userData ? _userData->getData() : _emptyMap;
		}

		static Session* getSession(const std::string& id);
		static void cleanupExpiredSessions();
		static void removeSession(const std::string& id);
};
