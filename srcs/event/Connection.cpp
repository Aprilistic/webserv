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

  eParseResult parserResult = mHttpParser.parseRequest(mRecvBuffer);
  if (parserResult == ParsingCompleted) {
    // Router router;
    // IEventHandler *handler = router.GetHandler(mHttpParser.getRequest());
    // // 적절한 핸들러를 통해 response 생성
    // Response response = handler->handle(request);

    ResponseMessage responseMessage;

    // // 생성된  response를 sendBuffer에 문법에 맞게 입력
    std::string message = responseMessage.getMessage();
    mSendBuffer.assign(message.begin(), message.end());

    mHttpParser.resetRequest();
    // buffer 초기화 확인
  } else if (parserResult == ParsingIncompleted) {
    return;
  } else if (parserResult == ParsingError) {
    // error
  }
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
