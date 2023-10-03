#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
	public:
		int mSocket;
		std::vector<char> mRecvBuffer;
  		std::vector<char> mSendBuffer;
	private:
};

#endif