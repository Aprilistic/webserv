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

public:
  std::map<int, SharedPtr<Connection> > mConnection;

private:
  int mSocket;
  int mPort;
  struct sockaddr_in mAddr;
};

#endif