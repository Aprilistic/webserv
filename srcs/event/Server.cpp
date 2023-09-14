#include "Server.hpp"

Server::Server(IOcontrol *IO, Node *ServerNode) : mIO(IO), mServerNode(ServerNode) {
  // mConfigTree 순회하면서 서버에 값 세팅

	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket < 0) {
		throw std::runtime_error("socket() failed");
	}

	fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	
	memset(&mAddr, 0, sizeof(mAddr));
	mAddr.sin_family = AF_INET;
	mAddr.sin_port = htons(mPort);
	mAddr.sin_addr.s_addr = htonl(0);

	if (bind(mSocket, (struct sockaddr *)&mAddr, sizeof(mAddr) < 0 ))
	{
		//error
	}

	if (listen(mSocket, 5 /* backlog size*/) < 0)
	{
		//error
	}
	
	// struct kevent evSet;
	// EV_SET(&evSet, mSocket, EVFILT_READ, EV_ADD, 0, 0, this);
	// if (kevent(mIO->kqueue, &evSet, 1, NULL, 0, NULL) < 0)
	// {
	// 	//error	
	// }

	struct kevent events[2];

	EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
	EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);
	kevent(mIO->kqueue, events, 2, NULL, 0, NULL);

	/* 
	1. kevent udata(마지막 인자)에 this를 넣어서 이벤트 발생시 this를 통해 해당 서버를 찾을 수 있게 한다.
	2. read, write handler에서 this를 통해 해당 서버를 찾아서 처리한다.
	3. 서버가 종료되면 this를 통해 해당 서버를 찾아서 종료시킨다.
	*/
}

Server::~Server() {}