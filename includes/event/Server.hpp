#ifndef SERVER_HPP
#define SERVER_HPP

#include "Core.hpp"
             
class Node;
class EventMonitor;
class Connection;
// class Server
// {
// 	public:
// 		Server(Node* mConfigTree);
// 		~Server();
// 	private:
// 		// http, server, location context
// 		std::map<int, std::string> mErrorPage; // ex) error_page
// <error_code> <path> -> error_page 404 500 /path/error.html; 이런값 받으면
// 자료형 바꿔야함 , default : x
//  		int mClientMaxBodySize; // default : 1m(1메가 바이트)
// 		std::vector<std::string> mIndex; // default : index.html
// 		bool mbAutoIndex; // on(1) / off(0) default : off

// 		// server context
// 		int mListen; // default : 80
// 		std::string mServerName; // default : default_server

// 		// server, location context
// 		std::map<int, std::string> mReturn; // default : x

// 		// location context
// 		std::string mAlias; // default : x
// 		std::vector<std::string> mLimitExcept; // default : x
// };

class Server {
public:
  Server(EventMonitor *Monitor, Node *ServerNode);
  ~Server();

  void ReadHandler();
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
  EventMonitor *mMonitor;
  Node *mServerNode;
  std::map<std::string, Node *> mLocationHashMap;
  std::map<int, Connection> mConnection;
  struct sockaddr_in mAddr;
  int mPort;
};

#endif