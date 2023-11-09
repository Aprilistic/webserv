#define SERVER_LEVEL (1 << 2)
#define LOCATION_LEVEL (1 << 3)

#include "Server.hpp"
#include "Connection.hpp"
#include "WebServer.hpp"

Server::Server(int port) : mPort(port) {
  mSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (mSocket < 0) {
    // FD limit exceededw
    throw std::runtime_error("Error: socket() creation failed: " +
                             std::string(strerror(errno)));
  }

  fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

  memset(&mAddr, 0, sizeof(mAddr));
  mAddr.sin_family = AF_INET;
  mAddr.sin_port = htons(mPort);

  mAddr.sin_addr.s_addr = htonl(0);
  // mAddr.sin_addr.s_addr = INADDR_ANY;

  int optval = 1;
  if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) <
      0) {
    throw std::runtime_error("Error: Failed to set socket option: " +
                             std::string(strerror(errno)));
  }

  if (bind(mSocket, (struct sockaddr *)&mAddr, sizeof(mAddr)) < 0) {
    throw std::runtime_error("Error: Failed to bind the socket: " +
                             std::string(strerror(errno)));
  }

  if (listen(mSocket, SOMAXCONN /* backlog size*/) < 0) {
    throw std::runtime_error("Error: Failed to listen on the socket: " +
                             std::string(strerror(errno)));
  }

  struct kevent events[2];

  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD, 0, 0, this);
  // EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);

  kevent(Common::mKqueue, events, 1, NULL, 0, NULL);

  //   struct timespec *timeout = NULL; // wait indefinitely
  //   int n = kevent(mWebServer->GetKqueue(), NULL, 0, events, 1, timeout);
}

Server::~Server() {
  // Remove the socket from the mKqueue
  struct kevent events[2];
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  // EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(Common::mKqueue, events, 1, NULL, 0, NULL);

  // Close the socket
  mConnection.clear();
  close(mSocket);
}

void Server::EventHandler(struct kevent &currentEvent) {
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
    assert("Server::EventHandler: default" == 0);
    break;
  }
}

void Server::readHandler() {
  int socket = accept(mSocket, NULL, NULL);
  if (socket == -1) {
    std::cout << RED << "Error: Failed to accept the connection: "
              << std::string(strerror(errno)) << RESET << std::endl;
  }

  mConnection[socket] = SharedPtr<Connection>(new Connection(socket, mPort));
}

void Server::writeHandler() {}

void Server::timerHandler() {}

void Server::signalHandler() {}
