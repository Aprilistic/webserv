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
  void startServer(void);
  void stopServer(void);

public:
private:
};

#endif