#include "ProgramControl.hpp"

void ProgramControl::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    // error
  }
  switch (currentEvent.filter) {
  case EVFILT_READ:
    assert("ProgramControl::EventHandler: EVFILT_READ" == 0);
    break;
  case EVFILT_WRITE:
    assert("ProgramControl::EventHandler: EVFILT_WRITE" == 0);
    break;
  case EVFILT_TIMER:
    assert("ProgramControl::EventHandler: EVFILT_TIMER" == 0);
    break;
  case EVFILT_SIGNAL:
    signalHandler(currentEvent.ident);
    break;
  default:
    assert("ProgramControl::EventHandler: default" == 0);
    break;
  }
}

void ProgramControl::RegisterTerminationSignals(void) {
  struct kevent event[3];

  EV_SET(&event[0], SIGINT, EVFILT_SIGNAL, EV_ADD, 0, 0, this);
  EV_SET(&event[1], SIGTERM, EVFILT_SIGNAL, EV_ADD, 0, 0, this);
  EV_SET(&event[2], SIGQUIT, EVFILT_SIGNAL, EV_ADD, 0, 0, this);
  kevent(Common::mKqueue, event, 3, NULL, 0, NULL);
}

void ProgramControl::UnregisterTerminationSignals(void) {
  struct kevent event[3];

  EV_SET(&event[0], SIGINT, EVFILT_SIGNAL, EV_DELETE, 0, 0, this);
  EV_SET(&event[1], SIGTERM, EVFILT_SIGNAL, EV_DELETE, 0, 0, this);
  EV_SET(&event[2], SIGQUIT, EVFILT_SIGNAL, EV_DELETE, 0, 0, this);
  kevent(Common::mKqueue, event, 3, NULL, 0, NULL);
}

void ProgramControl::signalHandler(int signal) {
  switch (signal) {
  case SIGINT:
  case SIGTERM:
  case SIGQUIT:
    Common::mRunning = false;
    break;
  default:
    assert("ProgramControl::signalHandler: default" == 0);
    break;
  }
}