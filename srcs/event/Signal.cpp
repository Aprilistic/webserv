#include "Signal.hpp"

void Signal::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    // error
  }
  switch (currentEvent.filter) {
  case EVFILT_READ:
		assert("Signal::EventHandler: EVFILT_READ" == 0);
    break;
  case EVFILT_WRITE:
		assert("Signal::EventHandler: EVFILT_WRITE" == 0);
    break;
  case EVFILT_TIMER:
		assert("Signal::EventHandler: EVFILT_TIMER" == 0);
    break;
  case EVFILT_SIGNAL:
		signalHandler(currentEvent.ident);
    break;
  default:
		assert("Signal::EventHandler: default" == 0);
    break;
  }
}