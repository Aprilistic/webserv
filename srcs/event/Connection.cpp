#include "Connection.hpp"
#include "IRequestHandler.hpp"
#include "Router.hpp"

Connection::Connection(int socket, int port) : mSocket(socket), mPort(port) {
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
  char tmp[RECV_BUFFER_SIZE];
  bytesRead = recv(mSocket, tmp, RECV_BUFFER_SIZE, 0);
  mRecvBuffer.insert(mRecvBuffer.end(), tmp, tmp + bytesRead);

  // std::cout << "bytesRead: " << bytesRead << std::endl;
  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      // error
    }
    // disconnection();
    return;
  }

  eStatusCode parseStatus = mHttp.parseRequest(mRecvBuffer);

  switch (parseStatus) {
  case (ParsingCompleted):
    break;
  case (ParsingIncompleted):
    return;
  default:
    return (mHttp.ErrorHandle(mPort, parseStatus));
  }

  // 포트가 같은데 둘 다 이름이 없는 경우 localhost로 접근할 때,
  // default_server로 안 가는 문제

  if (mHttp.CheckRedirect(mPort))
    return; // redirect path 로  response
  if (mHttp.checkClientMaxBodySize(mPort) == false)
    return (mHttp.ErrorHandle(mPort, CLIENT_ERROR_CONTENT_TOO_LARGE));
  if (mHttp.CheckLimitExcept(mPort) == false)
    return (mHttp.ErrorHandle(mPort, CLIENT_ERROR_METHOD_NOT_ALLOWED));

  IRequestHandler *handler = Router::Routing(mHttp);
  eStatusCode handleStatus = handler->handle(mPort, mHttp);

  switch (handleStatus) {
  case (SUCCESSFUL_OK):
  case (SUCCESSFUL_CREATERD):
    return; // response
  case (SUCCESSFUL_ACCEPTED):
    return; //
  default:
    return (mHttp.ErrorHandle(mPort, handleStatus));
  }

  ResponseMessage responseMessage(mHttp.getResponse());

  mSendBuffer = responseMessage.getMessageToVector();
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, &mSendBuffer[0], mSendBuffer.size(), 0);

  if (bytesSent <= 0) {
    if (bytesSent < 0) {
      // error
    }
    return;
  }
  mHttp.resetResponse();
}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}
