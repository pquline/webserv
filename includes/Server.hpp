#ifndef SERVER_HPP
#define SERVER_HPP

# include <iostream>
# include <string>

class Server
{
	private:

		std::string m_port;

	public:

		Server(const std::string &port);
		virtual ~Server ();

		int init();

};

#endif /* SERVER_HPP */
