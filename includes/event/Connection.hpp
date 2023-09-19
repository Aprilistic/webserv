#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Core.hpp"

class Connection
{
	public:
		Connection(int ClientSocket);
		~Connection();
	private:
		int mClientSocket;
		std::string mBuffer;
};

#endif