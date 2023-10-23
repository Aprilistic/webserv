#include "Connection.hpp"
#include "Node.hpp"
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

eStatusCode Connection::readFromSocket() {
  mRecvBuffer.clear();

  ssize_t bytesRead;
  char tmp[RECV_BUFFER_SIZE];
  bytesRead = recv(mSocket, tmp, RECV_BUFFER_SIZE, 0);
  mRecvBuffer.insert(mRecvBuffer.end(), tmp, tmp + bytesRead);

  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      // error
      return (SOCKET_READ_ERROR);
    }
    // disconnection();
    return (SOCKET_DISCONNECTED);
  }
  return (READ_OK);
}

void Connection::readHandler() {
  eStatusCode state = readFromSocket();
  switch (state) {
  case (SOCKET_READ_ERROR):
    mHttp.ErrorHandle(mPort, state);
    break;
  case (SOCKET_DISCONNECTED):
    /* code */
    break;
  case (READ_OK):
    state = mHttp.requestParser(mPort, mRecvBuffer);
    if (state == ERROR) {
      break;
    } else if (state == ParsingIncompleted) {
      return;
    }
  case (ParsingCompleted):
    state = mHttp.priorityHeaders(mPort);
    if (state == REDIRECT || state == ERROR) {
      break;
    }
  case (PRIORITY_HEADER_OK):
    state = mHttp.setResponse(mPort);
    if (state == RESPONSE_INCOMPLETED) {
      return;
    }
  default:
    break;
  }
  // 포트가 같은데 둘 다 이름이 없는 경우 localhost로 접근할 때,
  // default_server로 안 가는 문제'

  mHttp.MakeMandatoryHeaders();
  ResponseMessage responseMessage(mHttp.getResponse());

  std::string test = responseMessage.getMessage();
  std::cout << test << std::endl;

  mSendBuffer = responseMessage.getMessageToVector();

  // std::string httpResponse = "HTTP/1.1 200 OK\r\n"
  //                            "Content-Type: text/plain\r\n"
  //                            "\r\n"
  //                            "Hello, World!";

  // 문자열을 vector<char>에 담기
  // std::vector<char> responseVec(httpResponse.begin(), httpResponse.end());
  // mSendBuffer = responseVec;
  mHttp.resetRequest();
  mHttp.resetResponse();
  mHttp.resetRequestParser();
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, &mSendBuffer[0], mSendBuffer.size(), 0);

  mSendBuffer.clear();
  if (bytesSent <= 0) {
    if (bytesSent < 0) {
      // error
    }
    return;
  }
}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}
