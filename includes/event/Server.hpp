#ifndef SERVER_HPP
#define SERVER_HPP

#include "Core.hpp"
             
class Node;
class Master;
class Connection;

class Server {
public:
  Server(Master *master, Node *serverNode);

  ~Server();

  void ReadHandler(int ident);
  void WriteHandler();
  void TimerHandler();
  void SignalHandler();
private:
  Server();
  void makeHashMap(Node *curNode);
  void printHashMap(void);
  void makeLocationHashMap(Node *curNode);
  void deleteLocationHashMap(void);

public:
  int mSocket;
private:
  Master *mMaster;
  Node *mServerNode;
  std::map<std::string, Node *> mLocationHashMap;
  std::map<int, Connection *> mConnection;
  struct sockaddr_in mAddr;
  int mPort;
  std::vector<char> mRecvBuffer;
  std::vector<char> mSendBuffer;
};

#endif