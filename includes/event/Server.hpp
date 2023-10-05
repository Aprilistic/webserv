#ifndef Server_HPP
#define Server_HPP

#include "Core.hpp"
#include "IEventHandler.hpp"

class Node;
class WebServer;
class Connection;

class Server : public IEventHandler {
public:
  Server(Node *ServerNode);
  ~Server();

private:
  Server();
//   void makeHashMap(Node *curNode);
//   void printHashMap(void);
  void makeLocationHashMap(Node *curNode);
  virtual void HandleReadEvent();
  virtual void HandleWriteEvent();
  virtual void HandleTimerEvent();
  virtual void HandleSignalEvent();
//   void deleteLocationConfigHashMap(void);

public:
  int mSocket;
  std::map<std::string, Node *> mLocationHashMap;
  std::map<int, Connection *> mConnection;
private:
  Node *mServerNode;  //May not need this
  struct sockaddr_in mAddr;
  int mPort;
};

#endif