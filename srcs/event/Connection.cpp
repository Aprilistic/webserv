#include "Connection.hpp"

Connection::Connection(int socket) : mSocket(socket) {
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

#include "Node.hpp"

void Connection::readHandler() {
  mRecvBuffer.clear();
  ssize_t bytesRead;
  bytesRead = recv(mSocket, &mRecvBuffer[0], RECV_BUFFER_SIZE, 0);

  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      // error
    }
    // disconnection();
    return;
  }
  
  if (mHttpParser.parseRequest(mRecvBuffer) == ParsingIncompleted)
  {
	return ;
  }

  Node *test = Common::mConfigMap->GetConfigNode(80, "localhostt", "/");

  std::string str = "alias";
  std::cout << test->FindValue(test, str)[0] << std::endl;

  std::string str1 = "client_max_body_size";
  std::cout << test->FindValue(test, str1)[0] << std::endl;
  // if (mHttpParser.parseRequest(mRecvBuffer) == ParsingCompleted)
  // {
	// return ;
  // }

  // Router router;

  // IRequestHandler* handler =  router.Routing(mHttpParser.getRequest());

  // Response response = handler->handle(mHttpParser.getRequest());
 
  // mSendBuffer = mHttpParser.parseResponse(response);
  
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, &mSendBuffer[0], mSendBuffer.size(), 0);

  if (bytesSent <= 0) {
    if (bytesSent < 0) {
      // error
    }
    return;
  }
  mHttpParser.resetResponse();
}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}
