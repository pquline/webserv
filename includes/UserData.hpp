#pragma once

class UserData {
	private:
		std::string _id;
		std::map<std::string, std::string> _data;
		static std::map<std::string, UserData*> _users;

	public:
		UserData(const std::string& id);
		~UserData();

		void setAttribute(const std::string& name, const std::string& value);
		void removeAttribute(const std::string& name);
		std::string getAttribute(const std::string& name) const;
		const std::map<std::string, std::string>& getData() const { return _data; }

		static UserData* getUser(const std::string& id);
		static void removeUser(const std::string& id);
};
