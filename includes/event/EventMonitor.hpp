#ifndef EVENTMONITOR_HPP
#define EVENTMONITOR_HPP

#include "Config.hpp"
#include "Core.hpp"

class EventMonitor {
public:
  EventMonitor();
  ~EventMonitor();

private:
  int mKq;
  int mQuit;
  struct kevent evSet;
  struct kevent evList[32];

};

#endif