#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Config.hpp"
#include "Connection.hpp"
#include "Core.hpp"
#include "Server.hpp"

#define MAX_EVENT 1024

#define SESSION_TIMEOUT 100

#define RECV_BUFFER_SIZE 131072
#define SEND_BUFFER_SIZE 131072

class WebServer {
public:
  WebServer(const std::string &path);
  ~WebServer(void);
  void Run(void);
  bool IsGood(void) const;

private:
  void eventMonitoring(void);

private:
  bool mGood;
  std::map<int, Server *> mServerList;
  std::vector<struct kevent> mEventList;
};

#endif