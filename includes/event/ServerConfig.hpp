#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Core.hpp"

class Node;
class WebServer;
class Connection;

class ServerConfig {
public:
  ServerConfig(WebServer *webServer, Node *serverConfigNode);

  ~ServerConfig();

private:
  ServerConfig();
//   void makeHashMap(Node *curNode);
//   void printHashMap(void);
  void makeLocationConfigHashMap(Node *curNode);
//   void deleteLocationConfigHashMap(void);

public:
  int mSocket;
private:
  WebServer *mWebServer;
  Node *mServerConfigNode;
  std::map<std::string, Node *> mLocationConfigHashMap;
  std::map<int, Connection *> mConnection;
  struct sockaddr_in mAddr;
  int mPort;
  std::vector<char> mRecvBuffer;
  std::vector<char> mSendBuffer;
};

#endif