#ifndef MASTER_HPP
#define MASTER_HPP

#include "Config.hpp"
#include "Core.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class Master {
public:
  Master(const std::string &path);
  ~Master(void);
  void EventMonitoring(void);
  void EventHandler(struct kevent& currentEvent);

private:
//   Master();
  int mKqueue;
  std::vector<Server *> mServerList;
  std::vector<struct kevent> mEventList;

public:
  int GetKqueue(void) const;
};

#endif