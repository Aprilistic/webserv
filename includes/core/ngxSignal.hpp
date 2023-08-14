#ifndef NGXSIGNAL_HPP
#define NGXSIGNAL_HPP

#include "NgxCore.hpp"

#define NGX_SHUTDOWN_SIGNAL SIGQUIT
#define NGX_TERMINATE_SIGNAL SIGTERM
#define NGX_NOACCEPT_SIGNAL SIGWINCH
#define NGX_RECONFIGURE_SIGNAL SIGHUP

class NgxSignal {
public:
  static void InitSignal(void);

private:
  NgxSignal(void);
  ~NgxSignal(void);

  static void signalHandler(int processType, int signal);

  static void setSignal(int signal);
  static void clearSignal(void);

  static void setShutdownSignal(void);
  static void setTerminateSignal(void);
  static void setNoAcceptSignal(void);
  static void setReconfigureSignal(void);

public:
private:
};

#endif