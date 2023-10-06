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

void Signal::RegisterTerminationSignals(void) {
	struct kevent event;

	EV_SET(&event, SIGINT, EVFILT_SIGNAL, EV_ADD, 0, 0, this);
	kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
	EV_SET(&event, SIGTERM, EVFILT_SIGNAL, EV_ADD, 0, 0, this);
	kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
	EV_SET(&event, SIGQUIT, EVFILT_SIGNAL, EV_ADD, 0, 0, this);
	kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
}

void Signal::UnregisterTerminationSignals(void) {
	struct kevent event;

	EV_SET(&event, SIGINT, EVFILT_SIGNAL, EV_DELETE, 0, 0, this);
	kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
	EV_SET(&event, SIGTERM, EVFILT_SIGNAL, EV_DELETE, 0, 0, this);
	kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
	EV_SET(&event, SIGQUIT, EVFILT_SIGNAL, EV_DELETE, 0, 0, this);
	kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
}

void Signal::signalHandler(int signal) {
	switch (signal) {
	case SIGINT:
	case SIGTERM:
	case SIGQUIT:
		Common::mRunning = false;
		break;
	default:
		assert("Signal::signalHandler: default" == 0);
		break;
	}
}