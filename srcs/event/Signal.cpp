#include "Signal.hpp"

void Signal::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    // error
  }
  switch (currentEvent.filter) {
  case EVFILT_READ:
    ReadHandler();
    break;
  case EVFILT_WRITE:
    WriteHandler();
    break;
  case EVFILT_TIMER:
    TimerHandler();
    break;
  case EVFILT_SIGNAL:
    SignalHandler();
    break;
  default:
    break;
  }
}