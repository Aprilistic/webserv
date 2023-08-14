#ifndef NGXMASTER_HPP
#define NGXMASTER_HPP

#include "NgxConfig.hpp"
#include "NgxCore.hpp"
#include "NgxWorkerGroup.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class NgxMaster {
public:
  NgxMaster(void);
  ~NgxMaster(void);

private:
  void startNginx(void);
  void reloadNginx(void);
  void stopNginx(void);

  void handleSIGHUP(void);
  void handleSIGQUIT(void);

public:
private:
  NgxWorkerGroup *mWorkerGroup;
};

#endif