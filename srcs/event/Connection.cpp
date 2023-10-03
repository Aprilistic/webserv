#include "Connection.hpp"

Connection::Connection(int socket)
	: mSocket(socket)
{
	struct kevent events[2];

	mRecvBuffer.reserve(RECV_BUFFER_SIZE);
	mSendBuffer.reserve(SEND_BUFFER_SIZE);

	fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, this);
	EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE,0, 0, this);
	kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
}

Connection::~Connection()
{
	close(mSocket);
}

void Connection::HandleReadEvent()
{
	mRecvBuffer.clear();
	ssize_t bytesRead;
	bytesRead = recv(mSocket, &mRecvBuffer[0], RECV_BUFFER_SIZE, 0);

	if (bytesRead <= 0)
	{
		if (bytesRead < 0)
		{
			// error
		}
		// disconnection(); 
		return ;
	}
}

void Connection::HandleWriteEvent() 
{
    ssize_t bytesSent = send(mSocket, &mSendBuffer[0], mSendBuffer.size(), 0);
    
    if (bytesSent <= 0) 
    {
        if (bytesSent < 0) 
        {
			//error
        }
        return;
    }
}

void Connection::HandleTimerEvent()
{
	// error
}