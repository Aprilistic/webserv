#include "Connection.hpp"
#include "Node.hpp"
#include "Router.hpp"
#include "WebServer.hpp"

Connection::Connection(int socket, int port)
    : mSocket(socket), mPort(port), mKeepAlive(true), mRemainingRequest(0),
      mHttp(socket, port, mSendBuffer, mKeepAlive, mRemainingRequest) {
  struct kevent events[2];

  mRecvBuffer.reserve(RECV_BUFFER_SIZE);
  mSendBuffer.reserve(SEND_BUFFER_SIZE);

  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0,
         this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, this);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
}

Connection::~Connection() {}

void Connection::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_EOF) {
    disconnect();
    return;
  }
  if (currentEvent.flags & EV_ERROR) {
    return;
  }
  switch (currentEvent.filter) {
  case EVFILT_READ:
    readHandler();
    break;
  case EVFILT_WRITE:
    writeHandler();
    break;
  default:
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

  if (bytesRead == -1) {
    return (READ_OK);
  }
  disconnect();
  return (SERVER_ERROR_INTERNAL_SERVER_ERROR);
}

void Connection::readHandler() {

  eStatusCode state = readFromSocket();

  mHttp.SetRequest(state, mRecvBuffer);

  struct kevent event;
  EV_SET(&event, mSocket, EVFILT_WRITE, EV_ENABLE | EV_ADD, 0, 0, this);
  kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, mSendBuffer.c_str(), mSendBuffer.size(), 0);

  if (bytesSent == -1) {
    disconnect();
  } else {
    // bytesSent 만큼 벡터에서 제거
    mSendBuffer.erase(mSendBuffer.begin(), mSendBuffer.begin() + bytesSent);
  }

  if (mSendBuffer.empty()) {
    if (mKeepAlive == false && mRemainingRequest == 0) {
      disconnect();
      return;
    }
  }
}

void Connection::disconnect() {
  struct kevent events[2];
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);

  close(mSocket);
  Log(info, "Connection: Client " + ToString(mSocket) + " is disconnected");
}