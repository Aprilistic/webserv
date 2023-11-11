#ifndef PROGRAMCONTROL_HPP
#define PROGRAMCONTROL_HPP

#include "Common.hpp"
#include "Core.hpp"
#include "IEventHandler.hpp"

class ProgramControl : public IEventHandler {
public:
  virtual void EventHandler(struct kevent &currentEvent);
  ProgramControl();
  ~ProgramControl();
private:
  static void timerHandler();

  void eraseCookieFile();
};

#endif