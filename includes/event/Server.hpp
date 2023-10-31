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
  //   void makeHashMap(Node *curNode);
  //   void printHashMap(void);
  //   void deleteLocationConfigHashMap(void);
  // void makeLocationHashMap(Node *curNode);
	void readHandler();
	void writeHandler();
  void timerHandler();
  void signalHandler();

public:
  int mSocket;
  // std::map<int, Connection *> mConnection;
  std::map<int, SharedPtr<Connection>> mConnection;
private:
  struct sockaddr_in mAddr;
  int mPort;
};

#endif