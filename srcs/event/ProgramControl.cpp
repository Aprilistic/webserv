#include "ProgramControl.hpp"
#include "WebServer.hpp"


ProgramControl::ProgramControl() {
  struct kevent event;
  EV_SET(&event, 0, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, SESSION_TIMEOUT * 2, this);
  kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
}

void ProgramControl::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    return;
  }
  switch (currentEvent.filter) {
  case EVFILT_TIMER:
    timerHandler();
    break;
  default:
    break;
  }
}

void ProgramControl::timerHandler() {
  
}