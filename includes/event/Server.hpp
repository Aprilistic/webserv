#ifndef Server_HPP
#define Server_HPP

#include "Core.hpp"
#include "IEventHandler.hpp"

class Node;
class WebServer;
class Connection;

class Server : public IEventHandler {
public:
  Server(WebServer *webServer, Node *ServerNode);

  ~Server();

private:
  Server();
//   void makeHashMap(Node *curNode);
//   void printHashMap(void);
  void makeLocationHashMap(Node *curNode);
//   void deleteLocationConfigHashMap(void);

public:
  int mSocket;
private:
  WebServer *mWebServer;
  Node *mServerNode;  //May not need this
  std::map<std::string, Node *> mLocationHashMap;
  std::map<int, Connection *> mConnection;
  struct sockaddr_in mAddr;
  int mPort;
  std::vector<char> mRecvBuffer;
  std::vector<char> mSendBuffer;
};

#endif