#include "NgxSignal.hpp"

void NgxSignal::InitSignal(void) {
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGTERM);
  sigaddset(&set, SIGWINCH);
  sigaddset(&set, SIGHUP);
  sigprocmask(SIG_BLOCK, &set, NULL);
}

void NgxSignal::signalHandler(int ProcessType, int Signal) {

  switch (ProcessType) {
  case NGX_PROCESS_MASTER:
    switch (Signal) {
    case NGX_SHUTDOWN_SIGNAL:
      /* code */
      break;
    case NGX_TERMINATE_SIGNAL:
      /* code */
      break;
    case NGX_NOACCEPT_SIGNAL:
      /* code */
      break;
    case NGX_RECONFIGURE_SIGNAL:
      /* code */
      break;
    default:
      assert("NgxSignal::signalHandler: Signal is not defined");
      break;
    }
    break;

  case NGX_PROCESS_WORKER:
    switch (Signal) {
    case NGX_SHUTDOWN_SIGNAL:
      /* code */
      break;
    case NGX_TERMINATE_SIGNAL:
      /* code */
      break;
    case NGX_NOACCEPT_SIGNAL:
      /* code */
      break;
    case NGX_RECONFIGURE_SIGNAL:
      /* code */
      break;
    default:
      assert("NgxSignal::signalHandler: Signal is not defined");
      break;
    }
  default:
    assert("NgxSignal::signalHandler: ProcessType is not defined");
    break;
  }
}

void NgxSignal::setShutdownSignal(void) {
  struct sigaction sa;
  sa.sa_handler = NgxSignal::signalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGQUIT, &sa, NULL);
}

void NgxSignal::setTerminateSignal(void) {
  struct sigaction sa;
  sa.sa_handler = NgxSignal::signalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTERM, &sa, NULL);
}

void NgxSignal::setNoAcceptSignal(void) {
  struct sigaction sa;
  sa.sa_handler = NgxSignal::signalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, NULL);
}

void NgxSignal::setReconfigureSignal(void) {
  struct sigaction sa;
  sa.sa_handler = NgxSignal::signalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGHUP, &sa, NULL);
}
