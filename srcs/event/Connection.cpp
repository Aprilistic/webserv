#include "Connection.hpp"

/* test */
#include "Request.hpp"
#include "RequestParser.hpp"
#include "Router.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
/* test */

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

void Connection::EventHandler(struct kevent &currentEvent)
{
	if (currentEvent.flags & EV_ERROR)
	{
		// error
	}
	switch (currentEvent.filter)
	{
	case EVFILT_READ:
		ReadHandler();
		break;
	case EVFILT_WRITE:
		WriteHandler();
		break;
	case EVFILT_TIMER:
		TimerHandler();
		break;
	case EVFILT_SIGNAL:
		SignalHandler();
		break;
	default:
		assert("Connection::EventHandler: default" == 0);
		break;
	}
}

void Connection::ReadHandler()
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
 
	// Request request;
	// RequestParser requestParser;
	// if (requestParser.parse(request, mRecvBuffer) == ParsingCompleted/* 아직 미완성 */)
	// {
	// 	return ;
	// }
	// // 파싱 완료 후 request 정보에 맞는 handler 찾기
	// Router router;
	// IEventHandler* handler = router.GetHandler(request);

	// // 적절한 핸들러를 통해 response 생성
	// Response response = handler->handle(request);
	
	// ResponseBuilder responsBuilder

	// // 생성된  response를 sendBuffer에 문법에 맞게 입력
	// mSendBuffer = responsBuilder.build(response);
}

void Connection::WriteHandler() 
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

void Connection::TimerHandler()
{
	// error
}

void Connection::SignalHandler()
{
	// error
}

void Connection::SignalHandler()
{

}