#include "Connection.hpp"
#include "Node.hpp"
#include "Router.hpp"

Connection::Connection(int socket, int port)
    : mSocket(socket), mPort(port), mKeepAlive(true), mRemainingRequest(0),
      mHttp(socket, port, mSendBuffer, mKeepAlive, mRemainingRequest) {
  struct kevent events[2];

  mRecvBuffer.reserve(RECV_BUFFER_SIZE);
  mSendBuffer.reserve(SEND_BUFFER_SIZE);

  fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, this);
  kevent(Common::mKqueue, events, 1, NULL, 0, NULL);
}

Connection::~Connection() {}

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
  case EVFILT_PROC:
    // processHandler();
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
  do {
    bytesRead = recv(mSocket, tmp, RECV_BUFFER_SIZE, 0);
    mRecvBuffer.insert(mRecvBuffer.end(), tmp, tmp + bytesRead);
  } while (bytesRead > 0);

  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        disconnect();
        return (SERVER_ERROR_INTERNAL_SERVER_ERROR);
      }
      return (SERVER_ERROR_INTERNAL_SERVER_ERROR);
    }
    return (SERVER_SERVICE_UNAVAILABLE);
  }
  return (READ_OK);
}

void Connection::readHandler() {

  eStatusCode state = readFromSocket();

  //   mHttp.SetRequest(state, mPort, mSocket, mRecvBuffer);
  mHttp.SetRequest(state, mRecvBuffer);

  struct kevent event;
  EV_SET(&event, mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, this);
  kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, mSendBuffer.c_str(), mSendBuffer.size(),
                           0); //   mSendBuffer.clear();

  if (bytesSent == -1) {
    // 에러 처리
    std::cout << RED << "1" << RESET << std::endl;
    disconnect();
  } else {
    // bytesSent 만큼 벡터에서 제거
    mSendBuffer.erase(mSendBuffer.begin(), mSendBuffer.begin() + bytesSent);
  }

  if (mSendBuffer.empty()) {
    struct kevent event;
    EV_SET(&event, mSocket, EVFILT_WRITE, EV_DELETE, 0, 0, this);
    kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);

    if (mKeepAlive == false && mRemainingRequest == 0) {
      disconnect();
      return;
    }
  }
}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}

void Connection::disconnect() {

  struct kevent events[2];
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
  close(mSocket);
}