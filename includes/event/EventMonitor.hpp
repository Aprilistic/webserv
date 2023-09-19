#ifndef EVENTMONITOR_HPP
#define EVENTMONITOR_HPP

#include "Config.hpp"
#include "Core.hpp"


class Server;

class EventMonitor {
public:
  EventMonitor(Node *mConfigTree);
  ~EventMonitor();
  struct kevent GetEvList();

  void MonitorIO();

private:
  int mQuit;
  struct kevent mEvList[32];
  std::vector<Server *> mServerList;

public:
  int mKqueue;
};

#endif