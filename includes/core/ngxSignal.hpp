#ifndef NGXSIGNAL_HPP
#define NGXSIGNAL_HPP

#include "ngxCore.hpp"

#define NGX_SHUTDOWN_SIGNAL SIGQUIT
#define NGX_TERMINATE_SIGNAL SIGTERM
#define NGX_NOACCEPT_SIGNAL SIGWINCH
#define NGX_RECONFIGURE_SIGNAL SIGHUP

class ngxSignal {
public:
  static void initSignal(void);

private:
  ngxSignal(void);
  ~ngxSignal(void);
	
  static void signalHandler(int ProcessType, int Signal);

  static void setSignal(int Signal);
  static void clearSignal(void);

  static void setShutdownSignal(void);
  static void setTerminateSignal(void);
  static void setNoAcceptSignal(void);
  static void setReconfigureSignal(void);

public:
private:
};

#endif