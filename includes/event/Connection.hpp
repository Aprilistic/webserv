#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Core.hpp"
#include "Common.hpp"
#include "IEventHandler.hpp"

class Connection : public IEventHandler
{
	public:
		int mSocket;
		std::vector<char> mRecvBuffer;
  		std::vector<char> mSendBuffer;
	private:
};

#endif