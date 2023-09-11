#ifndef MASTER_HPP
#define MASTER_HPP

#include "Config.hpp"
#include "Core.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class Master {
public:
  Master(const std::string &path);
  ~Master(void);

private:
  void startMaster(void);
  void stopMaster(void);

public:
private:
  std::map<int, std::vector<Server *> > mServersMap;
};

#endif