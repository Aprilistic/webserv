#ifndef Server_HPP
#define Server_HPP

#include "Core.hpp"
#include "IEventHandler.hpp"

class Node;
class WebServer;
class Connection;

class Server : public IEventHandler {
public:
  Server(int port);
  ~Server();
  virtual void EventHandler(struct kevent &currentEvent);

private:
  Server();
  void readHandler();
  void writeHandler();
  void timerHandler();
  void signalHandler();

// Do we need mConnection to be public?
public:
  // std::map<int, Connection *> mConnection;
  std::map<int, SharedPtr<Connection> > mConnection;

private:
  int mSocket;
  int mPort;
  struct sockaddr_in mAddr;
};

#endif