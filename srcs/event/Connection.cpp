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
      return (SERVER_ERROR_INTERNAL_SERVER_ERROR);
    }
    // disconnection();
    return (SERVER_SERVICE_UNAVAILABLE);
  }
  return (READ_OK);
}

void Connection::readHandler() {

  eStatusCode state = readFromSocket();

  mHttp.ResetAll();
  mHttp.SetRequest(state, mPort, mRecvBuffer);
}

// void Connection::readHandler() {

//   eStatusCode state = readFromSocket();

//   mHttp.SetRequest(state, mPort, mRecvBuffer);

//   switch (state) {
//   case (SOCKET_READ_ERROR):
//     mHttp.ErrorHandle(mPort, state);
//     break;
//   case (SOCKET_DISCONNECTED):
//     mHttp.ErrorHandle(mPort, state);
//     break;
//   case (READ_OK):
//     state = mHttp.setOneRequest(mPort, mRecvBuffer);
//     if (state == ERROR) {
//       break;
//     } else if (state == PARSING_INCOMPLETED) {
//       return;
//     }
//   case (PARSING_COMPLETED):
//     state = mHttp.PriorityHeaders(mPort);
//     if (state == REDIRECT || state == ERROR) {
//       break;
//     }
//   case (PRIORITY_HEADER_OK):
//     state = mHttp.SetResponse(mPort);
//   default:
//     break;
//   }

//   mHttp.ResetRequestParser();
// }

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
