#include "Connection.hpp"
#include "Node.hpp"
#include "Router.hpp"

Connection::Connection(int socket, int port)
: mSocket(socket)
, mPort(port)
, mHttp(socket, port, mSendBuffer) {
  struct kevent events[2];

  mRecvBuffer.reserve(RECV_BUFFER_SIZE);
  mSendBuffer.reserve(SEND_BUFFER_SIZE);

  fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, this);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
}

Connection::~Connection() { close(mSocket); }

void Connection::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    // error
  }
  switch (currentEvent.filter) {
  case EVFILT_READ:
    readHandler();
    break;
  case EVFILT_WRITE:
    writeHandler();
    break;
  case EVFILT_TIMER:
    timerHandler();
    break;
  case EVFILT_SIGNAL:
    signalHandler();
    break;
  default:
    assert("Connection::EventHandler: default" == 0);
    break;
  }
}

eStatusCode Connection::readFromSocket() {
  mRecvBuffer.clear();

  ssize_t bytesRead;
  char tmp[RECV_BUFFER_SIZE];
  bytesRead = recv(mSocket, tmp, RECV_BUFFER_SIZE, 0);
  mRecvBuffer.insert(mRecvBuffer.end(), tmp, tmp + bytesRead);

  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      // error
      return (SERVER_ERROR_INTERNAL_SERVER_ERROR);
    }
    // disconnection();
    return (SERVER_SERVICE_UNAVAILABLE);
  }
  return (READ_OK);
}

void Connection::readHandler() {

  eStatusCode state = readFromSocket();

//   mHttp.SetRequest(state, mPort, mSocket, mRecvBuffer);
  mHttp.SetRequest(state, mRecvBuffer);

}

void Connection::writeHandler()
{
	ssize_t bytesSent = send(mSocket, mSendBuffer.c_str(), mSendBuffer.size(), 0);	//   mSendBuffer.clear();

	if (bytesSent == -1)
	{
		// 에러 처리
	}
	else
	{
		// bytesSent 만큼 벡터에서 제거
		mSendBuffer.erase(mSendBuffer.begin(), mSendBuffer.begin() + bytesSent);
	}	

}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}
