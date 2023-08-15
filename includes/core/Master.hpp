#ifndef MASTER_HPP
#define MASTER_HPP

#include "Config.hpp"
#include "Core.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class Master {
public:
  Master(void);
  ~Master(void);

private:
  void startServer(void);
  void reloadServer(void);
  void stopServer(void);

public:
private:
};

#endif